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
#include "enum_metadata.generated.h"
#include "native-aot.generated.header.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

namespace chaos::il2cpp::codegen::SolutionCorePack {

// Bring runtime_core and jit declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::jit;

extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_InvokeWorkload_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticDouble(void);
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticInt(void);
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArrayAccess(void);
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunLoopBranch(void);
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunObjectAlloc(void);
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunStringConcat(void);
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunVirtualCall(void);
extern "C" void SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder_Compute_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj_get_Value(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);

// Runtime prelude dependencies included at translation unit level


inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_ICalc = static_cast<CHAOS_IL2CPP_INTPTR>(12442021228370428464ULL);
const void* chaos_vtable_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder[];
const void* chaos_vtable_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj[];
inline MethodTable chaos_mt_System_Private_CoreLib_System_Object = {nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
static constexpr InterfaceMapEntry chaos_iface_map_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder[] = {
	{ chaos_type_id_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_ICalc, 1, 1 }
};
inline MethodTable chaos_mt_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder, 14262183984434432730ULL, 2u, 32, 1, 1, chaos_iface_map_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder, nullptr, 1, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder = static_cast<CHAOS_IL2CPP_INTPTR>(14262183984434432730ULL);
inline MethodTable chaos_mt_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj, 10857097966795336750ULL, 4u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj = static_cast<CHAOS_IL2CPP_INTPTR>(10857097966795336750ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Int32 = {nullptr, nullptr, 11009693519287992193ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Int32 = static_cast<CHAOS_IL2CPP_INTPTR>(11009693519287992193ULL);
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
inline MethodTable chaos_mt_System_Private_CoreLib_System_Type = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 3222174513575444759ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Type = static_cast<CHAOS_IL2CPP_INTPTR>(3222174513575444759ULL);
inline MethodTable chaos_mt_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_ICalc = {nullptr, nullptr, 12442021228370428464ULL, 0u, 32, 3, 0, nullptr, nullptr, 0, 0, 0, 0};

// ── Virtual method table arrays ──
extern "C" void SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder_Compute_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
const void* chaos_vtable_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder[] =
{
	reinterpret_cast<void*>(SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder__ctor),
	reinterpret_cast<void*>(SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder_Compute_System_Int32),
};
extern "C" void SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj_get_Value(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj[] =
{
	nullptr,
	nullptr,
	reinterpret_cast<void*>(SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj__ctor_System_Int32),
	reinterpret_cast<void*>(SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj_get_Value),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder[] =
{
	{ 0x00000013u, reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder__ctor) },
	{ 0x00000012u, reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder_Compute_System_Int32) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x00000010u, reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj__ctor_System_Int32) },
	{ 0x0000000Fu, reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj_get_Value) },
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

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INT32 field_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj___Value_k__BackingField = 0;
};

struct chaos_type_System_Private_CoreLib_System_Int32
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

struct chaos_type_System_Private_CoreLib_System_Type : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR runtime_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

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
	auto* chaos_combined_utf8 = new char[chaos_combined_length + 1];
	if (chaos_left_length != 0)
	{
		CHAOS_IL2CPP_MEMCPY(chaos_combined_utf8, chaos_left_utf8, chaos_left_length);
	}
	if (chaos_right_length != 0)
	{
		CHAOS_IL2CPP_MEMCPY(chaos_combined_utf8 + chaos_left_length, chaos_right_utf8, chaos_right_length);
	}
	chaos_combined_utf8[chaos_combined_length] = '\0';
	const auto chaos_result = chaos_reflection_create_string_utf8_copy(chaos_combined_utf8, chaos_combined_length);
	delete[] chaos_combined_utf8;
	return chaos_result;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_string_literal(const char* chaos_utf8_data)
{
	if (chaos_utf8_data == nullptr)
	{
		return 0;
	}

	return chaos_reflection_create_string_utf8_copy(chaos_utf8_data, CHAOS_IL2CPP_STRLEN(chaos_utf8_data));
}


// ── GcTypeLayout pointer offset arrays (computed via offsetof) ──
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Type[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_name_value))};

extern "C" void ChaosRegisterGcLayouts() {
	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();

	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(14262183984434432730ULL, sizeof(chaos_type_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder), nullptr, 0);
	registry.Register(10857097966795336750ULL, sizeof(chaos_type_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj), nullptr, 0);
	registry.Register(11009693519287992193ULL, sizeof(chaos_type_System_Private_CoreLib_System_Int32), nullptr, 0);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);
	registry.Register(3222174513575444759ULL, sizeof(chaos_type_System_Private_CoreLib_System_Type), kGcOffsets_chaos_type_System_Private_CoreLib_System_Type, 2);

	// Register MethodTable address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder), reinterpret_cast<uintptr_t>(&chaos_mt_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder) + sizeof(chaos_mt_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj), reinterpret_cast<uintptr_t>(&chaos_mt_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj) + sizeof(chaos_mt_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Int32), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Int32) + sizeof(chaos_mt_System_Private_CoreLib_System_Int32));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Type), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Type) + sizeof(chaos_mt_System_Private_CoreLib_System_Type));
}

CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	auto* chaos_value_slot = chaos_resolve_native_int_slot(chaos_arg_0);
	const CHAOS_IL2CPP_STRING chaos_formatted = ChaosIl2cpp::Common::format_int32(static_cast<CHAOS_IL2CPP_INT32>(*chaos_value_slot));
	const auto chaos_id = chaos::il2cpp::string_table::Intern(
		chaos_formatted.c_str(), static_cast<CHAOS_IL2CPP_UINT32>(chaos_formatted.size()));
	return chaos_make_string_id_value(chaos_id);
}

void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	return ChaosReflectionConcatStringPairValues(chaos_fn_arg_0, chaos_fn_arg_1);
}

CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	if (chaos_arg_0 == 0)
	{
		CHAOS_IL2CPP_FAIL();
	}

	if (chaos_is_string_id(chaos_arg_0))
	{
		return static_cast<CHAOS_IL2CPP_INT32>(
			chaos::il2cpp::string_table::Resolve(
				chaos_extract_string_id(chaos_arg_0)).byte_count);
	}

	auto* chaos_string = reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(chaos_arg_0);
	return static_cast<CHAOS_IL2CPP_INT32>(chaos_string->length);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };


// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: System.Private.CoreLib/System.Int32
extern "C" void chaos_bridge_thunk_0(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[0])();
}

// Bridge/import thunk for: SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+SmallObj::<Value>k__BackingField
extern "C" void chaos_bridge_thunk_1(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[5])();
}


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[13] = {
	0u,
	1u,
	1u,
	1u,
	1u,
	1u,
	1u,
	1u,
	1u,
	1u,
	2u,
	3u,
	3u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[12];
	CHAOS_IL2CPP_UINT8 params[3];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		12u,
		3u,
		2406783522u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 1u, 1u },  // SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_InvokeWorkload_System_Int32
		{ 1u, 0u },  // SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticDouble
		{ 1u, 0u },  // SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticInt
		{ 1u, 0u },  // SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArrayAccess
		{ 1u, 0u },  // SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunLoopBranch
		{ 1u, 0u },  // SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunObjectAlloc
		{ 1u, 0u },  // SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunStringConcat
		{ 1u, 0u },  // SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunVirtualCall
		{ 0u, 0u },  // SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder__ctor
		{ 10u, 1u },  // SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder_Compute_System_Int32
		{ 0u, 1u },  // SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj__ctor_System_Int32
		{ 1u, 0u },  // SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj_get_Value
	},
	{
		1u,
		1u,
		1u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[5] = {
		0u,
		1548u,
		1544u,
		1556u,
		1544u,
	};

	static constexpr const char* s_type_names[5] = {
		"<Module>",
		"SolutionCorePackBenchmarkWorkloads",
		"SmallObj",
		"ICalc",
		"Adder",
	};

	static constexpr const char* s_type_namespaces[5] = {
		"",
		"Chaos.Generated.ManagedTests.SolutionCorePack",
		"",
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
		chaos_mt_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj.AsTypeInfoHot(),
		chaos_mt_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_ICalc.AsTypeInfoHot(),
		chaos_mt_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder.AsTypeInfoHot(),
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[3] = {
		33554435u,
		33554436u,
		33554437u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[6] = {
		0u,
		0u,
		3u,
		3u,
		3u,
		3u,
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
		/* .name_utf8         = */ "SolutionCorePack",
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
		::chaos::il2cpp::runtime_core::RegisterModule("SolutionCorePack", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[12] = {
	{ "InvokeWorkload", 0x00000007u, 1u },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads
	{ "RunArithmeticDouble", 0x00000009u, 0u },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads
	{ "RunArithmeticInt", 0x00000008u, 0u },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads
	{ "RunArrayAccess", 0x0000000Bu, 0u },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads
	{ "RunLoopBranch", 0x0000000Eu, 0u },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads
	{ "RunObjectAlloc", 0x0000000Cu, 0u },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads
	{ "RunStringConcat", 0x0000000Au, 0u },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads
	{ "RunVirtualCall", 0x0000000Du, 0u },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads
	{ ".ctor", 0x00000013u, 0u },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+Adder
	{ "Compute", 0x00000012u, 1u },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+Adder
	{ ".ctor", 0x00000010u, 1u },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+SmallObj
	{ "get_Value", 0x0000000Fu, 0u },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+SmallObj
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[3] = {
	{ "Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads", "Chaos.Generated.ManagedTests.SolutionCorePack", 0u, 8u },
	{ "Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+Adder", "Chaos.Generated.ManagedTests.SolutionCorePack", 8u, 2u },
	{ "Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+SmallObj", "Chaos.Generated.ManagedTests.SolutionCorePack", 10u, 2u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[12] = {
	{ 0x00000007u, 0u },
	{ 0x00000008u, 2u },
	{ 0x00000009u, 1u },
	{ 0x0000000Au, 6u },
	{ 0x0000000Bu, 3u },
	{ 0x0000000Cu, 5u },
	{ 0x0000000Du, 7u },
	{ 0x0000000Eu, 4u },
	{ 0x0000000Fu, 11u },
	{ 0x00000010u, 10u },
	{ 0x00000012u, 9u },
	{ 0x00000013u, 8u },
};

// Dispatch table (function pointers)
// s_hotpatch_entries — external linkage for verification_dispatch
// NOTE: `static` is deliberately omitted so that the verification dispatch
// TU (verification_dispatch.generated.cpp) can access this array via
// GetHotpatchEntries(). Without external linkage, the linker cannot resolve
// the symbol across translation units.
extern "C" HotpatchEntryV0 s_hotpatch_entries[12] = {
	{ reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_InvokeWorkload_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::InvokeWorkload
	{ reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticDouble), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunArithmeticDouble
	{ reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticInt), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunArithmeticInt
	{ reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArrayAccess), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunArrayAccess
	{ reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunLoopBranch), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunLoopBranch
	{ reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunObjectAlloc), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunObjectAlloc
	{ reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunStringConcat), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunStringConcat
	{ reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunVirtualCall), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunVirtualCall
	{ reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+Adder::.ctor
	{ reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder_Compute_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+Adder::Compute
	{ reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj__ctor_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+SmallObj::.ctor
	{ reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj_get_Value), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+SmallObj::get_Value
};

// Accessor functions for verification_dispatch.generated.cpp
extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept { return s_hotpatch_entries; }
extern "C" int32_t GetHotpatchEntryCount() noexcept { return 12; }

// Module hotpatch bundle (extern linkage — referenced across page files)
extern constexpr HotpatchModuleV0 s_hotpatch_module = {
	"SolutionCorePack",
	s_hotpatch_types,
	3u,
	s_hotpatch_methods,
	12u,
	s_hotpatch_slots,
	12u,
	s_hotpatch_entries,
	12u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[6] = {
	"System.Private.CoreLib/System.Int32",
	"System.Private.CoreLib/System.Int32::ToString:System.String()",
	"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)",
	"System.Private.CoreLib/System.String::get_Length:System.Int32()",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
	"SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+SmallObj::<Value>k__BackingField",
};

extern "C" void* kChaosExternalRuntimeFnTable[6] = {
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 6;
// (no method AOT entries for this module)
// ── Method table (kMethodTable[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
// NOTE: Pure data only — dispatch routing is in <chaos/hotpatch_dispatch.h>.
// JIT mode uses direct_ptr (precode trampoline → JIT compilation).

static void (*kMethodTable[12])() = {
	reinterpret_cast<void(*)()>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_InvokeWorkload_System_Int32),
	reinterpret_cast<void(*)()>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticDouble),
	reinterpret_cast<void(*)()>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticInt),
	reinterpret_cast<void(*)()>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArrayAccess),
	reinterpret_cast<void(*)()>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunLoopBranch),
	reinterpret_cast<void(*)()>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunObjectAlloc),
	reinterpret_cast<void(*)()>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunStringConcat),
	reinterpret_cast<void(*)()>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunVirtualCall),
	reinterpret_cast<void(*)()>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder__ctor),
	reinterpret_cast<void(*)()>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder_Compute_System_Int32),
	reinterpret_cast<void(*)()>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj__ctor_System_Int32),
	reinterpret_cast<void(*)()>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj_get_Value),
};

// ── Default-arg thunks (kDefaultArgThunks[]) ────────────────────────────
// Each thunk supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_null_instance = 0;
extern "C" void (*kDefaultArgThunks[12])() = {
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[0])(0);},
	[]() {kMethodTable[1]();},
	[]() {kMethodTable[2]();},
	[]() {kMethodTable[3]();},
	[]() {kMethodTable[4]();},
	[]() {kMethodTable[5]();},
	[]() {kMethodTable[6]();},
	[]() {kMethodTable[7]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[8])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[9])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[10])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[11])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
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

	/* ── Entry 0: SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_InvokeWorkload_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry0;
	/* ── Entry 1: SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticDouble ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry1;
	/* ── Entry 2: SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticInt ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry2;
	/* ── Entry 3: SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArrayAccess ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry3;
	/* ── Entry 4: SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunLoopBranch ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry4;
	/* ── Entry 5: SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunObjectAlloc ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry5;
	/* ── Entry 6: SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunStringConcat ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry6;
	/* ── Entry 7: SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunVirtualCall ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry7;
	/* ── Entry 8: SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry8;
	/* ── Entry 9: SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder_Compute_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry9;
	/* ── Entry 10: SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj__ctor_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry10;
	/* ── Entry 11: SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj_get_Value ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry11;
} kChaosGcSlotMapsSection = {
	/* entry0 = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_InvokeWorkload_System_Int32 */
	.entry0 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_InvokeWorkload_System_Int32),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 8u, 16u }
	},
	/* entry1 = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticDouble */
	.entry1 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticDouble),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry2 = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticInt */
	.entry2 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticInt),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry3 = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArrayAccess */
	.entry3 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArrayAccess),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry4 = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunLoopBranch */
	.entry4 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunLoopBranch),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry5 = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunObjectAlloc */
	.entry5 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunObjectAlloc),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry6 = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunStringConcat */
	.entry6 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunStringConcat),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry7 = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunVirtualCall */
	.entry7 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunVirtualCall),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry8 = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder__ctor */
	.entry8 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry9 = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder_Compute_System_Int32 */
	.entry9 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder_Compute_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry10 = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj__ctor_System_Int32 */
	.entry10 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj__ctor_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry11 = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj_get_Value */
	.entry11 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj_get_Value),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 440u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[12] = {
	reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_InvokeWorkload_System_Int32),
	reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticDouble),
	reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticInt),
	reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArrayAccess),
	reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunLoopBranch),
	reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunObjectAlloc),
	reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunStringConcat),
	reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunVirtualCall),
	reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder__ctor),
	reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder_Compute_System_Int32),
	reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj__ctor_System_Int32),
	reinterpret_cast<void*>(&SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj_get_Value),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0xC5ED74C5017ACADA),
		0x00000004u,
		0u,
		2u,
		reinterpret_cast<const void*>(kSlots_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder),
		reinterpret_cast<const void**>(chaos_vtable_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder),
		2u,
		1,
		{0, 0, 0},
		chaos_iface_map_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder,
		1u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x96AC2913D273AC2E),
		0x00000002u,
		0u,
		4u,
		reinterpret_cast<const void*>(kSlots_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj),
		reinterpret_cast<const void**>(chaos_vtable_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj),
		4u,
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
	.method_pointer_count      = 12u,
	.reverse_pinvoke_wrappers  = nullptr,
	.reverse_pinvoke_wrapper_count = 0u,
	.invoker_pointers          = nullptr,
	.invoker_pointer_count     = 0u,
	.unresolved_virtual_calls = nullptr,
	.unresolved_virtual_call_count = 0u,
	.type_capabilities       = nullptr,
	.type_capability_count   = 0u,
	.vtable_descriptors = kChaosVTableDescriptors,
	.vtable_descriptor_count = 2u,
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
	.image_name_utf8    = "SolutionCorePack",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads[8] = {
	{ 0x00000007u, "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::InvokeWorkload:System.Int32(System.Int32)", "InvokeWorkload", "System.Int32", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000009u, "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunArithmeticDouble:System.Int32()", "RunArithmeticDouble", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000008u, "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunArithmeticInt:System.Int32()", "RunArithmeticInt", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Bu, "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunArrayAccess:System.Int32()", "RunArrayAccess", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Eu, "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunLoopBranch:System.Int32()", "RunLoopBranch", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Cu, "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunObjectAlloc:System.Int32()", "RunObjectAlloc", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Au, "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunStringConcat:System.Int32()", "RunStringConcat", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Du, "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunVirtualCall:System.Int32()", "RunVirtualCall", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder[2] = {
	{ 0x00000013u, "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+Adder::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000012u, "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+Adder::Compute:System.Int64(System.Int32)", "Compute", "System.Int64", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj[2] = {
	{ 0x00000010u, "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+SmallObj::.ctor:System.Void(System.Int32)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Fu, "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+SmallObj::get_Value:System.Int32()", "get_Value", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[3] = {
	{ 0x00000001u, "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads", "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads", "Chaos.Generated.ManagedTests.SolutionCorePack", "Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads", "Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads, 8u, nullptr, 0u, 0u },
	{ 0x00000004u, "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+Adder", "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+Adder", "Chaos.Generated.ManagedTests.SolutionCorePack", "Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+Adder", "Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+Adder", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder, 2u, nullptr, 0u, 0u },
	{ 0x00000002u, "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+SmallObj", "SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+SmallObj", "Chaos.Generated.ManagedTests.SolutionCorePack", "Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+SmallObj", "Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+SmallObj", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj, 2u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[3] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
};

static const ReflectionQueryImageDescriptor kReflImage = { "SolutionCorePack", kReflTypePtrs, 3u, 1, 0, 0, 0 };

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
// Managed method: SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::InvokeWorkload(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_InvokeWorkload_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
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
	CHAOS_IL2CPP_INTPTR _s6;
	CHAOS_IL2CPP_INTPTR _s7;
	CHAOS_IL2CPP_INTPTR _s8;
	CHAOS_IL2CPP_INTPTR _s9;
	CHAOS_IL2CPP_INTPTR _s10;
	_s0 = chaos_args[0];
	{
		const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(_s0);
		switch (chaos_switch_value)
		{
			case 0:
				{
					auto& _d2 = s_hotpatch_entries[2];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d2)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d2))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d2.method_key, nullptr, &_d_hpresult);
					}
					else
					{
						_d_hpresult = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticInt();
					}
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[0] = _s1;
			case 1:
				{
					auto& _d1 = s_hotpatch_entries[1];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d1)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d1))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d1.method_key, nullptr, &_d_hpresult);
					}
					else
					{
						_d_hpresult = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticDouble();
					}
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[0] = _s2;
			case 2:
				{
					auto& _d6 = s_hotpatch_entries[6];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d6.method_key, nullptr, &_d_hpresult);
					}
					else
					{
						_d_hpresult = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunStringConcat();
					}
					_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[0] = _s3;
			case 3:
				{
					auto& _d3 = s_hotpatch_entries[3];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d3)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d3))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d3.method_key, nullptr, &_d_hpresult);
					}
					else
					{
						_d_hpresult = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArrayAccess();
					}
					_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[0] = _s4;
			case 4:
				{
					auto& _d5 = s_hotpatch_entries[5];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d5)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d5))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d5.method_key, nullptr, &_d_hpresult);
					}
					else
					{
						_d_hpresult = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunObjectAlloc();
					}
					_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[0] = _s5;
			case 5:
				{
					auto& _d7 = s_hotpatch_entries[7];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d7.method_key, nullptr, &_d_hpresult);
					}
					else
					{
						_d_hpresult = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunVirtualCall();
					}
					_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[0] = _s6;
			case 6:
			{
				{
					auto& _d4 = s_hotpatch_entries[4];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d4)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d4))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d4.method_key, nullptr, &_d_hpresult);
					}
					else
					{
						_d_hpresult = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunLoopBranch();
					}
					_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[0] = _s7;
				break;
			}
			default:
			{
				{
					auto& _d2 = s_hotpatch_entries[2];
					CHAOS_IL2CPP_INT32 _d_hpresult{};
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d2)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d2))
					{
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d2.method_key, nullptr, &_d_hpresult);
					}
					else
					{
						_d_hpresult = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticInt();
					}
					_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
				}
				chaos_locals[0] = _s8;
				break;
			}
		}
	}
	{
		auto& _d2 = s_hotpatch_entries[2];
		CHAOS_IL2CPP_INT32 _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d2)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d2))
		{
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d2.method_key, nullptr, &_d_hpresult);
		}
		else
		{
			_d_hpresult = SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticInt();
		}
		_s9 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[0] = _s9;
	_s10 = chaos_locals[0];
	return static_cast<CHAOS_IL2CPP_INT32>(_s10);
}

// Managed method: SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunArithmeticDouble()
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticDouble(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};

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
	double _d0;
	double _d1;
	double _d2;
	double _d3;
	double _d4;
	double _d5;
	double _d6;
	double _d7;
	double _d8;
	double _d9;
	double _d10;
	double _d11;
	double _d12;
	_d0 = 0;
	chaos_locals[0] = ChaosStoreFloat64(_d0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[1] = _s0;
	do
	{
			_d1 = ChaosLoadFloat64(chaos_locals[0]);
			_s1 = chaos_locals[1];
			_d2 = static_cast<double>(_s1);
			_d3 = 3.14159;
			_d4 = (_d2 * _d3);
			_s2 = chaos_locals[1];
			_d5 = static_cast<double>(_s2);
			_d6 = 1;
			_d7 = (_d5 + _d6);
			_d8 = (_d4 / _d7);
			_d9 = (_d1 + _d8);
			chaos_locals[0] = ChaosStoreFloat64(_d9);
			_s3 = chaos_locals[1];
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s3), static_cast<CHAOS_IL2CPP_INT32>(_s4)));
			chaos_locals[1] = _s5;
			_s6 = chaos_locals[1];
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(10000);
			if (!(static_cast<CHAOS_IL2CPP_INT32>(_s6) < static_cast<CHAOS_IL2CPP_INT32>(_s7))) break;
	} while (true);
	_d10 = ChaosLoadFloat64(chaos_locals[0]);
	_d11 = 1000;
	_d12 = (_d10 * _d11);
	_s8 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_d12));
	_s9 = static_cast<CHAOS_IL2CPP_INTPTR>(10000);
	_s10 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s9));
	_s11 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosRem(static_cast<CHAOS_IL2CPP_INT32>(_s8), static_cast<CHAOS_IL2CPP_INT32>(_s10)));
	_s11 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s11));
	return static_cast<CHAOS_IL2CPP_INT32>(_s11);
}

// Managed method: SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunArithmeticInt()
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArithmeticInt(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};

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
	CHAOS_IL2CPP_INTPTR _s26;
	CHAOS_IL2CPP_INTPTR _s27;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = _s1;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[1] = _s2;
	do
	{
			_s3 = chaos_locals[0];
			_s4 = chaos_locals[1];
			_s5 = chaos_locals[1];
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s5), static_cast<CHAOS_IL2CPP_INT32>(_s6)));
			_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s4), static_cast<CHAOS_IL2CPP_INT32>(_s7)));
			_s9 = chaos_locals[1];
			_s10 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s11 = chaos_store_uint64(chaos_load_uint64(_s9) | chaos_load_uint64(_s10));
			_s12 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosDiv(static_cast<CHAOS_IL2CPP_INT32>(_s8), static_cast<CHAOS_IL2CPP_INT32>(_s11)));
			_s13 = chaos_locals[1];
			_s14 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s15 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_shift_right_int32(static_cast<CHAOS_IL2CPP_INT32>(_s13), static_cast<CHAOS_IL2CPP_INT32>(_s14)));
			_s16 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapSub(static_cast<CHAOS_IL2CPP_INT32>(_s12), static_cast<CHAOS_IL2CPP_INT32>(_s15)));
			_s17 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s16));
			_s18 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s3), static_cast<CHAOS_IL2CPP_INT32>(_s17)));
			chaos_locals[0] = _s18;
			_s19 = chaos_locals[1];
			_s20 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s21 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s19), static_cast<CHAOS_IL2CPP_INT32>(_s20)));
			chaos_locals[1] = _s21;
			_s22 = chaos_locals[1];
			_s23 = static_cast<CHAOS_IL2CPP_INTPTR>(10000);
			if (!(static_cast<CHAOS_IL2CPP_INT32>(_s22) < static_cast<CHAOS_IL2CPP_INT32>(_s23))) break;
	} while (true);
	_s24 = chaos_locals[0];
	_s25 = static_cast<CHAOS_IL2CPP_INTPTR>(2147483647);
	_s26 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s25));
	_s27 = chaos_store_uint64(chaos_load_uint64(_s24) & chaos_load_uint64(_s26));
	_s27 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s27));
	return static_cast<CHAOS_IL2CPP_INT32>(_s27);
}

// Managed method: SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunArrayAccess()
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunArrayAccess(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 4) chaos_locals{};

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
	CHAOS_IL2CPP_INTPTR _s26;
	CHAOS_IL2CPP_INTPTR _s27;
	CHAOS_IL2CPP_INTPTR _s28;
	CHAOS_IL2CPP_INTPTR _s29;
	CHAOS_IL2CPP_INTPTR _s30;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1000);
	{
		const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s0);
		if (chaos_length < 0)
		{
			CHAOS_IL2CPP_FAIL();
		}
		auto* chaos_array = CHAOS_IL2CPP_NEW_GC(chaos_managed_array, {});
		chaos_array->header.type_info = &chaos_type_info_managed_array.hot;
		chaos_array->element_type_shape = 1;
		chaos_array->element_type_info = chaos_mt_System_Private_CoreLib_System_Int32.AsTypeInfoHot();
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	chaos_locals[0] = _s1;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[2] = _s2;
	do
	{
			_s3 = chaos_locals[0];
			_s4 = chaos_locals[2];
			_s5 = chaos_locals[2];
			_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s5), static_cast<CHAOS_IL2CPP_INT32>(_s6)));
			{
				auto chaos_value_raw = _s7;
				const auto chaos_value = chaos_value_raw;
				const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s4);
				auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s3);
				if (chaos_array == nullptr)
				{
					CHAOS_IL2CPP_FAIL();
				}
				if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
				{
					CHAOS_IL2CPP_FAIL();
				}
				chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
			}
			_s8 = chaos_locals[2];
			_s9 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s10 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s8), static_cast<CHAOS_IL2CPP_INT32>(_s9)));
			chaos_locals[2] = _s10;
			_s11 = chaos_locals[2];
			_s12 = static_cast<CHAOS_IL2CPP_INTPTR>(1000);
			if (!(static_cast<CHAOS_IL2CPP_INT32>(_s11) < static_cast<CHAOS_IL2CPP_INT32>(_s12))) break;
	} while (true);
	_s13 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s14 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s13));
	chaos_locals[1] = _s14;
	_s15 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[3] = _s15;
	do
	{
			_s16 = chaos_locals[1];
			_s17 = chaos_locals[0];
			_s18 = chaos_locals[3];
			{
				const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s18);
				auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s17);
				if (chaos_array == nullptr)
				{
					CHAOS_IL2CPP_FAIL();
				}
				if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
				{
					CHAOS_IL2CPP_FAIL();
				}
				const auto chaos_element = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];
				_s19 = chaos_element;
			}
			_s20 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s19));
			_s21 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s16), static_cast<CHAOS_IL2CPP_INT32>(_s20)));
			chaos_locals[1] = _s21;
			_s22 = chaos_locals[3];
			_s23 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s24 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s22), static_cast<CHAOS_IL2CPP_INT32>(_s23)));
			chaos_locals[3] = _s24;
			_s25 = chaos_locals[3];
			_s26 = static_cast<CHAOS_IL2CPP_INTPTR>(1000);
			if (!(static_cast<CHAOS_IL2CPP_INT32>(_s25) < static_cast<CHAOS_IL2CPP_INT32>(_s26))) break;
	} while (true);
	_s27 = chaos_locals[1];
	_s28 = static_cast<CHAOS_IL2CPP_INTPTR>(2147483647);
	_s29 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s28));
	_s30 = chaos_store_uint64(chaos_load_uint64(_s27) & chaos_load_uint64(_s29));
	_s30 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s30));
	return static_cast<CHAOS_IL2CPP_INT32>(_s30);
}

// Managed method: SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunLoopBranch()
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunLoopBranch(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};

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
	CHAOS_IL2CPP_INTPTR _s26;
	CHAOS_IL2CPP_INTPTR _s27;
	CHAOS_IL2CPP_INTPTR _s28;
	CHAOS_IL2CPP_INTPTR _s29;
	CHAOS_IL2CPP_INTPTR _s30;
	CHAOS_IL2CPP_INTPTR _s31;
	CHAOS_IL2CPP_INTPTR _s32;
	CHAOS_IL2CPP_INTPTR _s33;
	CHAOS_IL2CPP_INTPTR _s34;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = _s1;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[1] = _s2;
	_s3 = chaos_locals[1];
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(10000);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(_s4);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(_s3);
		if (chaos_left >= chaos_right)
		{
			_s5 = chaos_locals[0];
			_s6 = chaos_locals[1];
			_s7 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s6));
			_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapSub(static_cast<CHAOS_IL2CPP_INT32>(_s5), static_cast<CHAOS_IL2CPP_INT32>(_s7)));
			chaos_locals[0] = _s8;
		}
		else
		{
			_s9 = chaos_locals[0];
			_s10 = chaos_locals[1];
			_s11 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s10));
			_s12 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s9), static_cast<CHAOS_IL2CPP_INT32>(_s11)));
			chaos_locals[0] = _s12;
		}
		_s13 = chaos_locals[1];
		_s14 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		_s15 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s13), static_cast<CHAOS_IL2CPP_INT32>(_s14)));
		chaos_locals[1] = _s15;
		do
		{
				_s16 = chaos_locals[1];
				_s17 = static_cast<CHAOS_IL2CPP_INTPTR>(10000);
				{
					const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(_s17);
					const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(_s16);
					if (chaos_left >= chaos_right)
					{
						_s18 = chaos_locals[0];
						_s19 = chaos_locals[1];
						_s20 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s19));
						_s21 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapSub(static_cast<CHAOS_IL2CPP_INT32>(_s18), static_cast<CHAOS_IL2CPP_INT32>(_s20)));
						chaos_locals[0] = _s21;
					}
					else
					{
						_s22 = chaos_locals[0];
						_s23 = chaos_locals[1];
						_s24 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s23));
						_s25 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s22), static_cast<CHAOS_IL2CPP_INT32>(_s24)));
						chaos_locals[0] = _s25;
					}
					_s26 = chaos_locals[1];
					_s27 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					_s28 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s26), static_cast<CHAOS_IL2CPP_INT32>(_s27)));
					chaos_locals[1] = _s28;
				}
				_s29 = chaos_locals[1];
				_s30 = static_cast<CHAOS_IL2CPP_INTPTR>(20000);
				if (!(static_cast<CHAOS_IL2CPP_INT32>(_s29) < static_cast<CHAOS_IL2CPP_INT32>(_s30))) break;
		} while (true);
		_s31 = chaos_locals[0];
		_s32 = static_cast<CHAOS_IL2CPP_INTPTR>(2147483647);
		_s33 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s32));
		_s34 = chaos_store_uint64(chaos_load_uint64(_s31) & chaos_load_uint64(_s33));
		_s34 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s34));
		return static_cast<CHAOS_IL2CPP_INT32>(_s34);
	}
}

// Managed method: SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunObjectAlloc()
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunObjectAlloc(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};

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
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = _s1;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[1] = _s2;
	do
	{
			_s3 = chaos_locals[0];
			_s4 = chaos_locals[1];
			{
				const auto chaos_arg_1 = _s4;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj, {});
				chaos_object->header.type_info = chaos_mt_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj.AsTypeInfoHot();
				SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj__ctor_System_Int32(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s5 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			{
				// Inlined: SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+SmallObj::get_Value:System.Int32()
				auto chaos_inline_arg_0 = _s5;
				CHAOS_IL2CPP_INTPTR chaos_inline_retval0{};
				_s6 = chaos_inline_arg_0;
				{
					auto* chaos_object = reinterpret_cast<chaos_type_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj*>(_s6);
					_s7 = chaos_object->field_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj___Value_k__BackingField;
				}
				chaos_inline_retval0 = _s7;
				goto chaos_inline_end0;
			chaos_inline_end0:
				;
				_s8 = chaos_inline_retval0;
			}
			_s9 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s8));
			_s10 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s3), static_cast<CHAOS_IL2CPP_INT32>(_s9)));
			chaos_locals[0] = _s10;
			_s11 = chaos_locals[1];
			_s12 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s13 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s11), static_cast<CHAOS_IL2CPP_INT32>(_s12)));
			chaos_locals[1] = _s13;
			_s14 = chaos_locals[1];
			_s15 = static_cast<CHAOS_IL2CPP_INTPTR>(5000);
			if (!(static_cast<CHAOS_IL2CPP_INT32>(_s14) < static_cast<CHAOS_IL2CPP_INT32>(_s15))) break;
	} while (true);
	_s16 = chaos_locals[0];
	_s17 = static_cast<CHAOS_IL2CPP_INTPTR>(2147483647);
	_s18 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s17));
	_s19 = chaos_store_uint64(chaos_load_uint64(_s16) & chaos_load_uint64(_s18));
	_s19 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s19));
	return static_cast<CHAOS_IL2CPP_INT32>(_s19);
}

// Managed method: SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunStringConcat()
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunStringConcat(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};

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
	{
		auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String);
		chaos_string->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();
		chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		chaos_string->utf8_data = "";
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
	}
	chaos_locals[0] = _s0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[1] = _s1;
	do
	{
			_s2 = chaos_locals[0];
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
				_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				const auto chaos_arg_1 = _s4;
				const auto chaos_arg_0 = _s2;
				const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
				_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[0] = _s5;
			_s6 = chaos_locals[1];
			_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s6), static_cast<CHAOS_IL2CPP_INT32>(_s7)));
			chaos_locals[1] = _s8;
			_s9 = chaos_locals[1];
			_s10 = static_cast<CHAOS_IL2CPP_INTPTR>(500);
			if (!(static_cast<CHAOS_IL2CPP_INT32>(_s9) < static_cast<CHAOS_IL2CPP_INT32>(_s10))) break;
	} while (true);
	_s11 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s11;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__(chaos_arg_0);
		_s12 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s12);
}

// Managed method: SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads::RunVirtualCall()
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_RunVirtualCall(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};

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
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder, {});
		chaos_object->header.type_info = chaos_mt_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder.AsTypeInfoHot();
		SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
	chaos_locals[1] = _s2;
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[2] = _s3;
	do
	{
			_s4 = chaos_locals[1];
			_s5 = chaos_locals[0];
			_s6 = chaos_locals[2];
			{
				const auto chaos_raw_arg_1 = _s6;
				const auto chaos_arg_1 = chaos_raw_arg_1;
				const auto chaos_raw_arg_0 = _s5;
				const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
				if (chaos_arg_0 == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				CHAOS_IL2CPP_INT64 chaos_callvirt_result{};
				chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 1u)))(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s7 = ChaosStoreInt64(chaos_callvirt_result);
			}
			_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s4), static_cast<CHAOS_IL2CPP_INT32>(_s7)));
			chaos_locals[1] = _s8;
			_s9 = chaos_locals[2];
			_s10 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s11 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s9), static_cast<CHAOS_IL2CPP_INT32>(_s10)));
			chaos_locals[2] = _s11;
			_s12 = chaos_locals[2];
			_s13 = static_cast<CHAOS_IL2CPP_INTPTR>(10000);
			if (!(static_cast<CHAOS_IL2CPP_INT32>(_s12) < static_cast<CHAOS_IL2CPP_INT32>(_s13))) break;
	} while (true);
	_s14 = chaos_locals[1];
	_s15 = static_cast<CHAOS_IL2CPP_INTPTR>(2147483647);
	_s16 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s15));
	_s17 = chaos_store_uint64(chaos_load_uint64(_s14) & chaos_load_uint64(_s16));
	_s17 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s17));
	return static_cast<CHAOS_IL2CPP_INT32>(_s17);
}

// Managed method: SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+Adder::.ctor()
extern "C" void SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+Adder::Compute(System.Int32)
extern "C" CHAOS_IL2CPP_INT64 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_Adder_Compute_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = chaos_args[1];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	_s3 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s2));
	return ChaosLoadInt64(_s3);
}

// Managed method: SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+SmallObj::.ctor(System.Int32)
extern "C" void SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj__ctor_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	_s1 = chaos_args[0];
	_s2 = chaos_args[1];
	{
		auto chaos_value = _s2;
		auto* chaos_object = reinterpret_cast<chaos_type_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj*>(_s1);
		chaos_object->field_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj___Value_k__BackingField = chaos_value;
	}
	return;
}

// Managed method: SolutionCorePack/Chaos.Generated.ManagedTests.SolutionCorePack.SolutionCorePackBenchmarkWorkloads+SmallObj::get_Value()
extern "C" CHAOS_IL2CPP_INT32 SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj_get_Value(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj*>(_s0);
		_s1 = chaos_object->field_SolutionCorePack_Chaos_Generated_ManagedTests_SolutionCorePack_SolutionCorePackBenchmarkWorkloads_SmallObj___Value_k__BackingField;
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s1);
}



}  // namespace chaos::il2cpp::codegen::SolutionCorePack
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 12;

extern "C" void ChaosJitRegisterAll() {}