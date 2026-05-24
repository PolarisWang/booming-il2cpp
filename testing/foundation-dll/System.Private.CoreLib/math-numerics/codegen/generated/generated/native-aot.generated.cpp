#include <chaos/common.h>
#include <chaos/type_info.h>
#include "runtime_core.h"
#include <chaos/eh.h>
#include "com_ccw.h"
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
#include "enum_metadata.generated.h"
#include "enum_stubs.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_0(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_1(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_10(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_11(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_12(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_13(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_14(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_15(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_16(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_17(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_2(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_3(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_4(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_5(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_6(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_7(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_8(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_9(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_0(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_1(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_10(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_11(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_12(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_13(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_14(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_15(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_16(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_17(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_2(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_3(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_4(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_5(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_6(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_7(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_8(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_9(void);

namespace chaos::il2cpp::codegen::MathNumericsSubjects {

// Bring runtime_core and jit declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::jit;

// Runtime prelude dependencies included at translation unit level


inline MethodTable chaos_mt_MathNumericsSubjects_MathNumericsSubjects = {nullptr, nullptr, 9582636018315310528ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_MathNumericsSubjects_MathNumericsSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(9582636018315310528ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Byte = {nullptr, nullptr, 3217969471912086765ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Byte = static_cast<CHAOS_IL2CPP_INTPTR>(3217969471912086765ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Object = {nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_String = {nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);

// ── Virtual method table arrays ──
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

struct chaos_type_MathNumericsSubjects_MathNumericsSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Byte
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Reflection_Assembly
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR runtime_assembly_name_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_method_handle = 0;
	CHAOS_IL2CPP_INTPTR generic_definition_method_handle = 0;
	CHAOS_IL2CPP_INTPTR generic_argument_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_method_handle = 0;
	CHAOS_IL2CPP_INTPTR generic_definition_method_handle = 0;
	CHAOS_IL2CPP_INTPTR generic_argument_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_String
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR length = 0;
	const char* utf8_data = nullptr;
	CHAOS_IL2CPP_UINT64 string_id = 0u;  // stable StringId, 0 = not yet materialized
};


// ── GcTypeLayout pointer offset arrays (computed via offsetof) ──
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};

extern "C" void ChaosRegisterGcLayouts() {
	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();

	registry.Register(9582636018315310528ULL, sizeof(chaos_type_MathNumericsSubjects_MathNumericsSubjects), nullptr, 0);
	registry.Register(3217969471912086765ULL, sizeof(chaos_type_System_Private_CoreLib_System_Byte), nullptr, 0);
	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);

	// Register MethodTable address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_MathNumericsSubjects_MathNumericsSubjects), reinterpret_cast<uintptr_t>(&chaos_mt_MathNumericsSubjects_MathNumericsSubjects) + sizeof(chaos_mt_MathNumericsSubjects_MathNumericsSubjects));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Byte), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Byte) + sizeof(chaos_mt_System_Private_CoreLib_System_Byte));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
}

CHAOS_IL2CPP_INT32 chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = 0;

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_BitConverter__GetBytes_System_Byte___System_Double_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	(void)chaos_arg_0;
	static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_BitConverter__GetBytes_System_Byte___System_Int32_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	return ChaosBitConverterGetBytes(0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
}

static CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Abs_System_Double_System_Double_(CHAOS_IL2CPP_INT64 chaos_fn_arg_0)
{
	return ChaosMathAbsDouble(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Math__Abs_System_Int32_System_Int32_(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	return ChaosMathAbsInt32(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__BigMul_System_Int64_System_Int32_System_Int32_(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	return ChaosMathBigMul(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Ceiling_System_Double_System_Double_(CHAOS_IL2CPP_INT64 chaos_fn_arg_0)
{
	return ChaosMathCeiling(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Cos_System_Double_System_Double_(CHAOS_IL2CPP_INT64 chaos_fn_arg_0)
{
	return ChaosMathCos(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Floor_System_Double_System_Double_(CHAOS_IL2CPP_INT64 chaos_fn_arg_0)
{
	return ChaosMathFloor(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Max_System_Double_System_Double_System_Double_(CHAOS_IL2CPP_INT64 chaos_fn_arg_0, CHAOS_IL2CPP_INT64 chaos_fn_arg_1)
{
	return ChaosMathMaxDouble(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Math__Max_System_Int32_System_Int32_System_Int32_(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	return ChaosMathMaxInt32(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Math__Min_System_Int32_System_Int32_System_Int32_(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	return ChaosMathMinInt32(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Pow_System_Double_System_Double_System_Double_(CHAOS_IL2CPP_INT64 chaos_fn_arg_0, CHAOS_IL2CPP_INT64 chaos_fn_arg_1)
{
	return ChaosMathPow(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Round_System_Double_System_Double_(CHAOS_IL2CPP_INT64 chaos_fn_arg_0)
{
	return ChaosMathRound(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Sin_System_Double_System_Double_(CHAOS_IL2CPP_INT64 chaos_fn_arg_0)
{
	return ChaosMathSin(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_Math__Sqrt_System_Double_System_Double_(CHAOS_IL2CPP_INT64 chaos_fn_arg_0)
{
	return ChaosMathSqrt(chaos_fn_arg_0);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };


// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: MathNumericsSubjects/MathNumericsSubjects::_exitCode
extern "C" void chaos_bridge_thunk_0(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[1])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Byte
extern "C" void chaos_bridge_thunk_1(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[16])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)
extern "C" CHAOS_IL2CPP_INT32 chaos_bridge_thunk_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[17])(chaos_fn_arg_0, chaos_fn_arg_1);
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)
extern "C" double chaos_bridge_thunk_3(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	auto result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_fn_arg_0, chaos_fn_arg_1);
	return result;
}

extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_0(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_1(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_2(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_3(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_4(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_5(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_6(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_7(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_8(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_9(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_10(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_11(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_12(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_13(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_14(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_15(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_16(void);
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_17(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[19] = {
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[18];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		18u,
		0u,
		400696213u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_0
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_1
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_2
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_3
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_4
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_5
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_6
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_7
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_8
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_9
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_10
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_11
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_12
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_13
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_14
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_15
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_16
		{ 0u, 0u },  // MathNumericsSubjects_MathNumericsSubjects_Subject_17
	},
	{
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[2] = {
		0u,
		1548u,
	};

	static constexpr const char* s_type_names[2] = {
		"<Module>",
		"MathNumericsSubjects",
	};

	static constexpr const char* s_type_namespaces[2] = {
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[2] = {
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[2] = {
		nullptr,
		chaos_mt_MathNumericsSubjects_MathNumericsSubjects.AsTypeInfoHot(),
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[3] = {
		0u,
		0u,
		0u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[3] = {
		0u,
		0u,
		0u,
	};



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "MathNumericsSubjects",
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
		/* .type_count        = */ 2u,
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
		::chaos::il2cpp::runtime_core::RegisterModule("MathNumericsSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[18] = {
	{ "Subject_0", 0x00000003u, 0u },  // MathNumericsSubjects
	{ "Subject_1", 0x00000004u, 0u },  // MathNumericsSubjects
	{ "Subject_2", 0x00000005u, 0u },  // MathNumericsSubjects
	{ "Subject_3", 0x00000006u, 0u },  // MathNumericsSubjects
	{ "Subject_4", 0x00000007u, 0u },  // MathNumericsSubjects
	{ "Subject_5", 0x00000008u, 0u },  // MathNumericsSubjects
	{ "Subject_6", 0x00000009u, 0u },  // MathNumericsSubjects
	{ "Subject_7", 0x0000000Au, 0u },  // MathNumericsSubjects
	{ "Subject_8", 0x0000000Bu, 0u },  // MathNumericsSubjects
	{ "Subject_9", 0x0000000Cu, 0u },  // MathNumericsSubjects
	{ "Subject_10", 0x0000000Du, 0u },  // MathNumericsSubjects
	{ "Subject_11", 0x0000000Eu, 0u },  // MathNumericsSubjects
	{ "Subject_12", 0x0000000Fu, 0u },  // MathNumericsSubjects
	{ "Subject_13", 0x00000010u, 0u },  // MathNumericsSubjects
	{ "Subject_14", 0x00000011u, 0u },  // MathNumericsSubjects
	{ "Subject_15", 0x00000012u, 0u },  // MathNumericsSubjects
	{ "Subject_16", 0x00000013u, 0u },  // MathNumericsSubjects
	{ "Subject_17", 0x00000014u, 0u },  // MathNumericsSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "MathNumericsSubjects", "", 0u, 18u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[18] = {
	{ 0x00000003u, 0u },
	{ 0x00000004u, 1u },
	{ 0x00000005u, 2u },
	{ 0x00000006u, 3u },
	{ 0x00000007u, 4u },
	{ 0x00000008u, 5u },
	{ 0x00000009u, 6u },
	{ 0x0000000Au, 7u },
	{ 0x0000000Bu, 8u },
	{ 0x0000000Cu, 9u },
	{ 0x0000000Du, 10u },
	{ 0x0000000Eu, 11u },
	{ 0x0000000Fu, 12u },
	{ 0x00000010u, 13u },
	{ 0x00000011u, 14u },
	{ 0x00000012u, 15u },
	{ 0x00000013u, 16u },
	{ 0x00000014u, 17u },
};

// Dispatch table (function pointers)
// s_hotpatch_entries — external linkage for verification_dispatch
// NOTE: `static` is deliberately omitted so that the verification dispatch
// TU (verification_dispatch.generated.cpp) can access this array via
// GetHotpatchEntries(). Without external linkage, the linker cannot resolve
// the symbol across translation units.
extern "C" HotpatchEntryV0 s_hotpatch_entries[18] = {
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_0
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_1
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_2
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_3
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_4
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_5
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_6
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_7
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_8
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_9
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_10
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_11
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_12
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_13
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_14
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_15
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_16
	{ reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // MathNumericsSubjects::Subject_17
};

// Accessor functions for verification_dispatch.generated.cpp
extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept { return s_hotpatch_entries; }
extern "C" int32_t GetHotpatchEntryCount() noexcept { return 18; }

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"MathNumericsSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	18u,
	s_hotpatch_slots,
	18u,
	s_hotpatch_entries,
	18u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[19] = {
	"System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)",
	"MathNumericsSubjects/MathNumericsSubjects::_exitCode",
	"System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)",
	"System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)",
	"System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)",
	"System.Private.CoreLib/System.Math::Round:System.Double(System.Double)",
	"System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)",
	"System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)",
	"System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)",
	"System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)",
	"System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)",
	"System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)",
	"System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)",
	"System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)",
	"System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)",
	"System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)",
	"System.Private.CoreLib/System.Byte",
	"System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)",
	"System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)",
};

extern "C" void* kChaosExternalRuntimeFnTable[19] = {
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Math__Abs_System_Int32_System_Int32_),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Math__Abs_System_Double_System_Double_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Math__Ceiling_System_Double_System_Double_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Math__Floor_System_Double_System_Double_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Math__Round_System_Double_System_Double_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Math__Max_System_Int32_System_Int32_System_Int32_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Math__Max_System_Double_System_Double_System_Double_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Math__Min_System_Int32_System_Int32_System_Int32_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Math__Sqrt_System_Double_System_Double_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Math__Pow_System_Double_System_Double_System_Double_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Math__Sin_System_Double_System_Double_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Math__Cos_System_Double_System_Double_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Math__BigMul_System_Int64_System_Int32_System_Int32_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_BitConverter__GetBytes_System_Byte___System_Int32_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_BitConverter__GetBytes_System_Byte___System_Double_),
	nullptr,
	nullptr,
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 19;
// (no method AOT entries for this module)
// ── Method table (kMethodTable[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
// NOTE: Pure data only — dispatch routing is in <chaos/hotpatch_dispatch.h>.
// JIT mode uses direct_ptr (precode trampoline → JIT compilation).

// ── Subject slot map (kSubjectSlotMap[]) ────────────────────────────
// Maps subject index (0-based sequential) to kMethodTable index.
// External consumers use this to route --benchmark N to the correct
// AOT method slot, since kMethodTable[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 18;
extern "C" const int kSubjectSlotMap[18] = {
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

	/* ── Entry 0: MathNumericsSubjects_MathNumericsSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry0;
	/* ── Entry 1: MathNumericsSubjects_MathNumericsSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry1;
	/* ── Entry 2: MathNumericsSubjects_MathNumericsSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry2;
	/* ── Entry 3: MathNumericsSubjects_MathNumericsSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry3;
	/* ── Entry 4: MathNumericsSubjects_MathNumericsSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry4;
	/* ── Entry 5: MathNumericsSubjects_MathNumericsSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry5;
	/* ── Entry 6: MathNumericsSubjects_MathNumericsSubjects_Subject_6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry6;
	/* ── Entry 7: MathNumericsSubjects_MathNumericsSubjects_Subject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry7;
	/* ── Entry 8: MathNumericsSubjects_MathNumericsSubjects_Subject_8 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry8;
	/* ── Entry 9: MathNumericsSubjects_MathNumericsSubjects_Subject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry9;
	/* ── Entry 10: MathNumericsSubjects_MathNumericsSubjects_Subject_10 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry10;
	/* ── Entry 11: MathNumericsSubjects_MathNumericsSubjects_Subject_11 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry11;
	/* ── Entry 12: MathNumericsSubjects_MathNumericsSubjects_Subject_12 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry12;
	/* ── Entry 13: MathNumericsSubjects_MathNumericsSubjects_Subject_13 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry13;
	/* ── Entry 14: MathNumericsSubjects_MathNumericsSubjects_Subject_14 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry14;
	/* ── Entry 15: MathNumericsSubjects_MathNumericsSubjects_Subject_15 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry15;
	/* ── Entry 16: MathNumericsSubjects_MathNumericsSubjects_Subject_16 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry16;
	/* ── Entry 17: MathNumericsSubjects_MathNumericsSubjects_Subject_17 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry17;
} kChaosGcSlotMapsSection = {
	/* entry0 = MathNumericsSubjects_MathNumericsSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry1 = MathNumericsSubjects_MathNumericsSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_1),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry2 = MathNumericsSubjects_MathNumericsSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_2),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry3 = MathNumericsSubjects_MathNumericsSubjects_Subject_3 */
	.entry3 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_3),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry4 = MathNumericsSubjects_MathNumericsSubjects_Subject_4 */
	.entry4 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_4),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry5 = MathNumericsSubjects_MathNumericsSubjects_Subject_5 */
	.entry5 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_5),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry6 = MathNumericsSubjects_MathNumericsSubjects_Subject_6 */
	.entry6 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_6),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry7 = MathNumericsSubjects_MathNumericsSubjects_Subject_7 */
	.entry7 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_7),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry8 = MathNumericsSubjects_MathNumericsSubjects_Subject_8 */
	.entry8 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_8),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry9 = MathNumericsSubjects_MathNumericsSubjects_Subject_9 */
	.entry9 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_9),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry10 = MathNumericsSubjects_MathNumericsSubjects_Subject_10 */
	.entry10 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_10),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry11 = MathNumericsSubjects_MathNumericsSubjects_Subject_11 */
	.entry11 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_11),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry12 = MathNumericsSubjects_MathNumericsSubjects_Subject_12 */
	.entry12 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_12),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry13 = MathNumericsSubjects_MathNumericsSubjects_Subject_13 */
	.entry13 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_13),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry14 = MathNumericsSubjects_MathNumericsSubjects_Subject_14 */
	.entry14 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_14),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry15 = MathNumericsSubjects_MathNumericsSubjects_Subject_15 */
	.entry15 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_15),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry16 = MathNumericsSubjects_MathNumericsSubjects_Subject_16 */
	.entry16 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_16),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry17 = MathNumericsSubjects_MathNumericsSubjects_Subject_17 */
	.entry17 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_17),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 732u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[18] = {
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_0),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_1),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_2),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_3),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_4),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_5),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_6),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_7),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_8),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_9),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_10),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_11),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_12),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_13),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_14),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_15),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_16),
	reinterpret_cast<void*>(&MathNumericsSubjects_MathNumericsSubjects_Subject_17),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 18u,
	.reverse_pinvoke_wrappers  = nullptr,
	.reverse_pinvoke_wrapper_count = 0u,
	.invoker_pointers          = nullptr,
	.invoker_pointer_count     = 0u,
	.unresolved_virtual_calls = nullptr,
	.unresolved_virtual_call_count = 0u,
	.type_capabilities       = nullptr,
	.type_capability_count   = 0u,
	.vtable_descriptors = nullptr,
	.vtable_descriptor_count = 0u,
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
	.image_name_utf8    = "MathNumericsSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_MathNumericsSubjects_MathNumericsSubjects[1] = {
	{ 0x04000001u, "MathNumericsSubjects/MathNumericsSubjects::_exitCode", "_exitCode", "System.Int32", 0LL, 3u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_MathNumericsSubjects_MathNumericsSubjects[18] = {
	{ 0x00000003u, "MathNumericsSubjects/MathNumericsSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000004u, "MathNumericsSubjects/MathNumericsSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000005u, "MathNumericsSubjects/MathNumericsSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000006u, "MathNumericsSubjects/MathNumericsSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000007u, "MathNumericsSubjects/MathNumericsSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000008u, "MathNumericsSubjects/MathNumericsSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000009u, "MathNumericsSubjects/MathNumericsSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Au, "MathNumericsSubjects/MathNumericsSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Bu, "MathNumericsSubjects/MathNumericsSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Cu, "MathNumericsSubjects/MathNumericsSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Du, "MathNumericsSubjects/MathNumericsSubjects::Subject_10:System.Void()", "Subject_10", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Eu, "MathNumericsSubjects/MathNumericsSubjects::Subject_11:System.Void()", "Subject_11", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Fu, "MathNumericsSubjects/MathNumericsSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000010u, "MathNumericsSubjects/MathNumericsSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000011u, "MathNumericsSubjects/MathNumericsSubjects::Subject_14:System.Void()", "Subject_14", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000012u, "MathNumericsSubjects/MathNumericsSubjects::Subject_15:System.Void()", "Subject_15", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000013u, "MathNumericsSubjects/MathNumericsSubjects::Subject_16:System.Void()", "Subject_16", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000014u, "MathNumericsSubjects/MathNumericsSubjects::Subject_17:System.Void()", "Subject_17", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0x00000001u, "MathNumericsSubjects/MathNumericsSubjects", "MathNumericsSubjects/MathNumericsSubjects", "", "MathNumericsSubjects", "MathNumericsSubjects", nullptr, kReflFields_MathNumericsSubjects_MathNumericsSubjects, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_MathNumericsSubjects_MathNumericsSubjects, 18u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "MathNumericsSubjects", kReflTypePtrs, 1u, 1, 0, 0, 0 };

// Fake ImageHandle that ResolveSubjectId will decode back to kReflImage.
// BootstrapRuntime's aot_image_handle fallback discovers this via
// LookupModule(mid)->image at lines 311-321 of bootstrap.cpp.
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
// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_0()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosMathAbsInt32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosMathAbsInt32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_1()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosMathAbsDouble(ChaosLoadInt64(chaos_arg_0));
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s0)));
			_s1 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosMathAbsDouble(ChaosLoadInt64(chaos_arg_0));
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s1)));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_2()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosMathCeiling(ChaosLoadInt64(chaos_arg_0));
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s0)));
			_s1 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosMathCeiling(ChaosLoadInt64(chaos_arg_0));
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s1)));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_3()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosMathFloor(ChaosLoadInt64(chaos_arg_0));
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s0)));
			_s1 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosMathFloor(ChaosLoadInt64(chaos_arg_0));
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s1)));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_4()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosMathRound(ChaosLoadInt64(chaos_arg_0));
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s0)));
			_s1 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosMathRound(ChaosLoadInt64(chaos_arg_0));
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s1)));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_5()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosMathRound(ChaosLoadInt64(chaos_arg_0));
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s0)));
			_s1 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosMathRound(ChaosLoadInt64(chaos_arg_0));
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s1)));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_6()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosMathMaxInt32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosMathMaxInt32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_7()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = ChaosStoreFloat64(42);
			_s1 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosMathMaxDouble(ChaosLoadInt64(chaos_arg_0), ChaosLoadInt64(chaos_arg_1));
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s0)));
			_s1 = ChaosStoreFloat64(42);
			_s2 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosMathMaxDouble(ChaosLoadInt64(chaos_arg_0), ChaosLoadInt64(chaos_arg_1));
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s1)));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosLoadFloat64(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_8()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosMathMinInt32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosMathMinInt32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_9()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosMathSqrt(ChaosLoadInt64(chaos_arg_0));
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s0)));
			_s1 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosMathSqrt(ChaosLoadInt64(chaos_arg_0));
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s1)));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_10()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = ChaosStoreFloat64(42);
			_s1 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosMathPow(ChaosLoadInt64(chaos_arg_0), ChaosLoadInt64(chaos_arg_1));
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s0)));
			_s1 = ChaosStoreFloat64(42);
			_s2 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosMathPow(ChaosLoadInt64(chaos_arg_0), ChaosLoadInt64(chaos_arg_1));
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s1)));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosLoadFloat64(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_11()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosMathSin(ChaosLoadInt64(chaos_arg_0));
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s0)));
			_s1 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosMathSin(ChaosLoadInt64(chaos_arg_0));
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s1)));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_12()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosMathCos(ChaosLoadInt64(chaos_arg_0));
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s0)));
			_s1 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosMathCos(ChaosLoadInt64(chaos_arg_0));
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s1)));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_13()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosMathBigMul(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s0 = ChaosStoreInt64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = ChaosMathBigMul(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s1 = ChaosStoreInt64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_14()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_14(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_BitConverter__GetBytes_System_Byte___System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = [&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(_s0); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }();
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s0));
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_BitConverter__GetBytes_System_Byte___System_Int32_(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = [&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(_s1); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }();
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_15()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_15(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};


	CHAOS_EH_TRY
			_s0 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_BitConverter__GetBytes_System_Byte___System_Double_(ChaosLoadInt64(chaos_arg_0));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = [&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(_s0); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }();
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s0)));
			_s1 = ChaosStoreFloat64(42);
			{
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_BitConverter__GetBytes_System_Byte___System_Double_(ChaosLoadInt64(chaos_arg_0));
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = [&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(_s1); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }();
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s1)));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_16()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_16(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
			{
				const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s0);
				if (chaos_length < 0)
				{
					CHAOS_IL2CPP_FAIL();
				}
				auto* chaos_array = CHAOS_IL2CPP_NEW_GC(chaos_managed_array, {});
				chaos_array->header.type_info = &chaos_type_info_managed_array.hot;
				chaos_array->element_type_shape = 1;
				chaos_array->element_type_info = chaos_mt_System_Private_CoreLib_System_Byte.AsTypeInfoHot();
				chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
				chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[17])(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
			{
				const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s1);
				if (chaos_length < 0)
				{
					CHAOS_IL2CPP_FAIL();
				}
				auto* chaos_array = CHAOS_IL2CPP_NEW_GC(chaos_managed_array, {});
				chaos_array->header.type_info = &chaos_type_info_managed_array.hot;
				chaos_array->element_type_shape = 1;
				chaos_array->element_type_info = chaos_mt_System_Private_CoreLib_System_Byte.AsTypeInfoHot();
				chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
				chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[17])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: MathNumericsSubjects/MathNumericsSubjects::Subject_17()
extern "C" void MathNumericsSubjects_MathNumericsSubjects_Subject_17(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};
	CHAOS_IL2CPP_INTPTR _s9{};
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(8);
			{
				const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s0);
				if (chaos_length < 0)
				{
					CHAOS_IL2CPP_FAIL();
				}
				auto* chaos_array = CHAOS_IL2CPP_NEW_GC(chaos_managed_array, {});
				chaos_array->header.type_info = &chaos_type_info_managed_array.hot;
				chaos_array->element_type_shape = 1;
				chaos_array->element_type_info = chaos_mt_System_Private_CoreLib_System_Byte.AsTypeInfoHot();
				chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
				chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_arg_0, chaos_arg_1);
				_s0 = ChaosStoreFloat64(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s0)));
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(8);
			{
				const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s1);
				if (chaos_length < 0)
				{
					CHAOS_IL2CPP_FAIL();
				}
				auto* chaos_array = CHAOS_IL2CPP_NEW_GC(chaos_managed_array, {});
				chaos_array->header.type_info = &chaos_type_info_managed_array.hot;
				chaos_array->element_type_shape = 1;
				chaos_array->element_type_info = chaos_mt_System_Private_CoreLib_System_Byte.AsTypeInfoHot();
				chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
				chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
			}
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[18])(chaos_arg_0, chaos_arg_1);
				_s1 = ChaosStoreFloat64(chaos_result);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s1)));
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_MathNumericsSubjects_MathNumericsSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}



}  // namespace chaos::il2cpp::codegen::MathNumericsSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 18;

// --- JIT Method Entry Table ---
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kJitJson_0[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-0\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_0:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_0()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_0:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_0:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-0\",\"signature\":\"System.Void MathNumericsSubjects::Subject_0()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":33,\"handlerOffset\":34,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":4,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":18,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":31,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":26,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":45,\"ilOffset\":32,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":34,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":36,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":37,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":45,\"ilOffset\":43,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":45,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_1[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-1\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_1:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_1()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_1:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_1:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-1\",\"signature\":\"System.Void MathNumericsSubjects::Subject_1()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":49,\"handlerOffset\":50,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":17,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":26,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":37,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":38,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":47,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":41,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":42,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":48,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":50,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":53,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":61,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_2[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-2\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_2:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_2()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_2:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_2:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-2\",\"signature\":\"System.Void MathNumericsSubjects::Subject_2()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":49,\"handlerOffset\":50,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":17,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":26,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":37,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":38,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":47,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":41,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":42,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":48,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":50,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":53,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":61,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_3[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-3\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_3:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_3()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_3:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_3:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-3\",\"signature\":\"System.Void MathNumericsSubjects::Subject_3()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":49,\"handlerOffset\":50,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":17,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":26,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":37,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":38,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":47,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":41,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":42,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":48,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":50,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":53,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":61,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_4[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-4\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_4:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_4()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_4:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_4:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-4\",\"signature\":\"System.Void MathNumericsSubjects::Subject_4()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":49,\"handlerOffset\":50,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Round:System.Double(System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Round:System.Double(System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Round:System.Double(System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":17,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":26,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Round:System.Double(System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Round:System.Double(System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Round:System.Double(System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":37,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":38,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":47,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":41,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":42,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":48,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":50,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":53,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":61,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_5[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-5\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_5:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_5()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_5:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_5:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-5\",\"signature\":\"System.Void MathNumericsSubjects::Subject_5()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":49,\"handlerOffset\":50,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Round:System.Double(System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Round:System.Double(System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Round:System.Double(System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":17,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":26,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Round:System.Double(System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Round:System.Double(System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Round:System.Double(System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":37,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":38,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":47,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":41,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":42,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":48,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":50,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":53,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":61,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_6[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-6\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_6:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_6()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_6:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_6:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-6\",\"signature\":\"System.Void MathNumericsSubjects::Subject_6()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":37,\"handlerOffset\":38,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":4,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":49,\"ilOffset\":36,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":38,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":40,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":41,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":49,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":49,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_7[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-7\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_7:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_7()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_7:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_7:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-7\",\"signature\":\"System.Void MathNumericsSubjects::Subject_7()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":67,\"handlerOffset\":68,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":11,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":26,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":35,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":44,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":49,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":50,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":53,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":55,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":56,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":65,\"ilOffset\":57,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":59,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":60,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":79,\"ilOffset\":66,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":68,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":70,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":71,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":79,\"ilOffset\":77,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":79,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_8[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-8\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_8:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_8()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_8:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_8:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-8\",\"signature\":\"System.Void MathNumericsSubjects::Subject_8()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":37,\"handlerOffset\":38,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":4,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":13,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":49,\"ilOffset\":36,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":38,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":40,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":41,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":49,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":49,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_9[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-9\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_9:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_9()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_9:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_9:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-9\",\"signature\":\"System.Void MathNumericsSubjects::Subject_9()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":49,\"handlerOffset\":50,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":17,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":26,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":37,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":38,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":47,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":41,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":42,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":48,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":50,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":53,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":61,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_10[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-10\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_10:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_10()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_10:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_10:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-10\",\"signature\":\"System.Void MathNumericsSubjects::Subject_10()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_10\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":67,\"handlerOffset\":68,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":11,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":26,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":35,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":44,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":49,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":50,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":53,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":55,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":56,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":65,\"ilOffset\":57,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":59,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":60,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":79,\"ilOffset\":66,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":68,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":70,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":71,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":79,\"ilOffset\":77,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":79,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_11[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-11\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_11:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_11()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_11:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_11:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-11\",\"signature\":\"System.Void MathNumericsSubjects::Subject_11()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_11\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":49,\"handlerOffset\":50,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":17,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":26,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":37,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":38,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":47,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":41,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":42,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":48,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":50,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":53,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":61,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_12[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-12\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_12:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_12()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_12:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_12:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-12\",\"signature\":\"System.Void MathNumericsSubjects::Subject_12()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_12\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":49,\"handlerOffset\":50,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":17,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":26,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":32,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":37,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":38,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":47,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":41,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":42,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":48,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":50,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":52,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":53,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":61,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":61,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_13[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-13\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_13:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_13()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_13:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_13:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-13\",\"signature\":\"System.Void MathNumericsSubjects::Subject_13()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_13\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":39,\"handlerOffset\":40,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":4,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.Int64\",\"callee\":\"System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":16,\"resultType\":\"System.Int64\",\"callee\":\"System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":24,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":25,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":37,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":32,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":51,\"ilOffset\":38,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":40,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":42,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":43,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":51,\"ilOffset\":49,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":51,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_14[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-14\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_14:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_14()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_14:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_14:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-14\",\"signature\":\"System.Void MathNumericsSubjects::Subject_14()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_14\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":37,\"handlerOffset\":38,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":4,\"resultType\":\"System.Byte[]\",\"callee\":\"System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":9,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":10,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.Byte[]\",\"callee\":\"System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":18,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":19,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":35,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":30,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":49,\"ilOffset\":36,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":38,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":40,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":41,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":49,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":49,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_15[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-15\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_15:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_15()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_15:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_15:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-15\",\"signature\":\"System.Void MathNumericsSubjects::Subject_15()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_15\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":51,\"handlerOffset\":52,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":2,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":11,\"resultType\":\"System.Byte[]\",\"callee\":\"System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":16,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":17,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":42,\"ilOffset\":18,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":27,\"resultType\":\"System.Byte[]\",\"callee\":\"System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":32,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":34,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":36,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":40,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":49,\"ilOffset\":41,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":43,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":44,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":63,\"ilOffset\":50,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":52,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":54,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":55,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":63,\"ilOffset\":61,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":63,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_16[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-16\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_16:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_16()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_16:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_16:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-16\",\"signature\":\"System.Void MathNumericsSubjects::Subject_16()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_16\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":43,\"handlerOffset\":44,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":3,\"resultType\":\"System.Byte[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":15,\"resultType\":\"System.Byte[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"callee\":\"System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":26,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":41,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":35,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":46,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":47,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":55,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":55,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";
static const char kJitJson_17[] = "{\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-17\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_17:System.Void()\",\"signature\":\"System.Void MathNumericsSubjects::Subject_17()\",\"identity\":{\"assemblyName\":\"MathNumericsSubjects\",\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"definitionSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_17:System.Void()\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::Subject_17:System.Void()\",\"methodId\":\"math-numerics-subjects.math-numerics-subjects.subject-17\",\"signature\":\"System.Void MathNumericsSubjects::Subject_17()\"},\"nativeSymbol\":\"MathNumericsSubjects_MathNumericsSubjects_Subject_17\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":45,\"handlerOffset\":46,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":8,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":3,\"resultType\":\"System.Byte[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":9,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":8,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"newarr\",\"opCode\":43,\"operand\":\"System.Private.CoreLib/System.Byte\",\"ilOffset\":16,\"resultType\":\"System.Byte[]\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Byte\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Byte\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":6,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":21,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":22,\"resultType\":\"System.Double\",\"callee\":\"System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":30,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":31,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":33,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":34,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":43,\"ilOffset\":35,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":38,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":57,\"ilOffset\":44,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":46,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":48,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"ilOffset\":49,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"MathNumericsSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"MathNumericsSubjects\",\"subjectId\":\"MathNumericsSubjects/MathNumericsSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"MathNumericsSubjects/MathNumericsSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":57,\"ilOffset\":55,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":57,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"MathNumericsSubjects\"}";

extern "C" const JitEntry kChaosJitEntries[18] =
{
    { kJitJson_0, 5226u, 0x00000003u, 0u },
    { kJitJson_1, 5430u, 0x00000004u, 0u },
    { kJitJson_2, 5454u, 0x00000005u, 0u },
    { kJitJson_3, 5442u, 0x00000006u, 0u },
    { kJitJson_4, 5442u, 0x00000007u, 0u },
    { kJitJson_5, 5442u, 0x00000008u, 0u },
    { kJitJson_6, 5512u, 0x00000009u, 0u },
    { kJitJson_7, 5724u, 0x0000000Au, 0u },
    { kJitJson_8, 5512u, 0x0000000Bu, 0u },
    { kJitJson_9, 5436u, 0x0000000Cu, 0u },
    { kJitJson_10, 5732u, 0x0000000Du, 0u },
    { kJitJson_11, 5438u, 0x0000000Eu, 0u },
    { kJitJson_12, 5438u, 0x0000000Fu, 0u },
    { kJitJson_13, 5724u, 0x00000010u, 0u },
    { kJitJson_14, 5690u, 0x00000011u, 0u },
    { kJitJson_15, 5700u, 0x00000012u, 0u },
    { kJitJson_16, 6653u, 0x00000013u, 0u },
    { kJitJson_17, 6853u, 0x00000014u, 0u }
};

extern "C" const uint32_t kChaosJitEntryCount = 18u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitEntryMethods(kChaosJitEntries, kChaosJitEntryCount);
}