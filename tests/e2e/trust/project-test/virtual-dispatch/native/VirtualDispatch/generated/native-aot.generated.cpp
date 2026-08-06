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
#include "runtime_stubs/stub_common.h"
#include "string_table.h"
#include "runtime_instantiation.h"
#include "reflection_query_model.h"
#include "load_store_chaos_bridge.h"
#include "interpreter_entry.h"
#include <gc/gc_bgc_inline.h>
#include <gc/gc_card_table.h>
#include <ChaosGeneratedRuntimePrelude.h>
#include "enum_metadata.generated.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

namespace chaos::il2cpp::codegen::VirtualDispatch {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_VirtualDispatch_IColoredShape = static_cast<CHAOS_IL2CPP_INTPTR>(6102461367829036461ULL);
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_VirtualDispatch_IShape = static_cast<CHAOS_IL2CPP_INTPTR>(14719592555683243611ULL);
const void* chaos_vtable_VirtualDispatch_Shape[];
const void* chaos_vtable_VirtualDispatch_Circle[];
const void* chaos_vtable_VirtualDispatch_Rectangle[];
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {{nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {{nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);
inline TypeInfoV0 chaos_mt_VirtualDispatch_Shape = {{nullptr, chaos_vtable_VirtualDispatch_Shape, 8449617212785324204ULL, 2u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_VirtualDispatch_Shape = static_cast<CHAOS_IL2CPP_INTPTR>(8449617212785324204ULL);
static constexpr InterfaceMapEntry chaos_iface_map_VirtualDispatch_Circle[] = {
	{ chaos_type_id_VirtualDispatch_IColoredShape, 4, 1 },
	{ chaos_type_id_VirtualDispatch_IShape, 1, 2 }
};
inline TypeInfoV0 chaos_mt_VirtualDispatch_Circle = {{&chaos_mt_VirtualDispatch_Shape.hot, chaos_vtable_VirtualDispatch_Circle, 10671939555361403883ULL, 5u, 32, 1, 1}, {chaos_iface_map_VirtualDispatch_Circle, nullptr, 2, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_VirtualDispatch_Circle = static_cast<CHAOS_IL2CPP_INTPTR>(10671939555361403883ULL);
static constexpr InterfaceMapEntry chaos_iface_map_VirtualDispatch_Rectangle[] = {
	{ chaos_type_id_VirtualDispatch_IShape, 1, 2 }
};
inline TypeInfoV0 chaos_mt_VirtualDispatch_Rectangle = {{&chaos_mt_VirtualDispatch_Shape.hot, chaos_vtable_VirtualDispatch_Rectangle, 10606890938084466472ULL, 6u, 32, 1, 1}, {chaos_iface_map_VirtualDispatch_Rectangle, nullptr, 1, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_VirtualDispatch_Rectangle = static_cast<CHAOS_IL2CPP_INTPTR>(10606890938084466472ULL);
inline TypeInfoV0 chaos_mt_VirtualDispatch_IColoredShape = {{nullptr, nullptr, 6102461367829036461ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline TypeInfoV0 chaos_mt_VirtualDispatch_IShape = {{nullptr, nullptr, 14719592555683243611ULL, 0u, 32, 3, 0}, {nullptr, nullptr, 0, 0, 0, 0}};

// ── Virtual method table arrays ──
extern "C" void VirtualDispatch_Shape__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR VirtualDispatch_Shape_GetName(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_VirtualDispatch_Shape[] =
{
	reinterpret_cast<void*>(VirtualDispatch_Shape__ctor),
	reinterpret_cast<void*>(VirtualDispatch_Shape_GetName),
};
extern "C" void VirtualDispatch_Shape__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR VirtualDispatch_Circle_GetName(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void VirtualDispatch_Circle__ctor_System_Double(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, double chaos_fn_arg_1);
extern "C" double VirtualDispatch_Circle_Area(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR VirtualDispatch_Circle_GetColor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_VirtualDispatch_Circle[] =
{
	reinterpret_cast<void*>(VirtualDispatch_Shape__ctor),
	reinterpret_cast<void*>(VirtualDispatch_Circle_GetName),
	reinterpret_cast<void*>(VirtualDispatch_Circle__ctor_System_Double),
	reinterpret_cast<void*>(VirtualDispatch_Circle_Area),
	reinterpret_cast<void*>(VirtualDispatch_Circle_GetColor),
};
extern "C" void VirtualDispatch_Shape__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR VirtualDispatch_Rectangle_GetName(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" double VirtualDispatch_Rectangle_Area(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void VirtualDispatch_Rectangle__ctor_System_Double_System_Double(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, double chaos_fn_arg_1, double chaos_fn_arg_2);
const void* chaos_vtable_VirtualDispatch_Rectangle[] =
{
	reinterpret_cast<void*>(VirtualDispatch_Shape__ctor),
	reinterpret_cast<void*>(VirtualDispatch_Rectangle_GetName),
	nullptr,
	reinterpret_cast<void*>(VirtualDispatch_Rectangle_Area),
	nullptr,
	reinterpret_cast<void*>(VirtualDispatch_Rectangle__ctor_System_Double_System_Double),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_VirtualDispatch_Shape[] =
{
	{ 0x00000014u, reinterpret_cast<void*>(&VirtualDispatch_Shape__ctor) },
	{ 0x00000013u, reinterpret_cast<void*>(&VirtualDispatch_Shape_GetName) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_VirtualDispatch_Circle[] =
{
	{ 0x00000014u, reinterpret_cast<void*>(&VirtualDispatch_Shape__ctor) },
	{ 0x00000016u, reinterpret_cast<void*>(&VirtualDispatch_Circle_GetName) },
	{ 0x00000015u, reinterpret_cast<void*>(&VirtualDispatch_Circle__ctor_System_Double) },
	{ 0x00000017u, reinterpret_cast<void*>(&VirtualDispatch_Circle_Area) },
	{ 0x00000018u, reinterpret_cast<void*>(&VirtualDispatch_Circle_GetColor) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_VirtualDispatch_Rectangle[] =
{
	{ 0x00000014u, reinterpret_cast<void*>(&VirtualDispatch_Shape__ctor) },
	{ 0x0000001Au, reinterpret_cast<void*>(&VirtualDispatch_Rectangle_GetName) },
	{ 0u, nullptr },
	{ 0x0000001Bu, reinterpret_cast<void*>(&VirtualDispatch_Rectangle_Area) },
	{ 0u, nullptr },
	{ 0x00000019u, reinterpret_cast<void*>(&VirtualDispatch_Rectangle__ctor_System_Double_System_Double) },
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
		return chaos_array->element_type_shape == chaos_type_shape_reference            && chaos_is_type_compatible(&chaos_mt_System_Private_CoreLib_System_String.hot, chaos_array->element_type_info);
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

struct chaos_type_VirtualDispatch_Shape
{
	ThinLockableHeader header{};
};

struct chaos_type_VirtualDispatch_Circle : public chaos_type_VirtualDispatch_Shape
{
	CHAOS_IL2CPP_INTPTR field_VirtualDispatch_Circle___radius = 0;
};

struct chaos_type_VirtualDispatch_Rectangle : public chaos_type_VirtualDispatch_Shape
{
	CHAOS_IL2CPP_INTPTR field_VirtualDispatch_Rectangle___h = 0;
	CHAOS_IL2CPP_INTPTR field_VirtualDispatch_Rectangle___w = 0;
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
	chaos_string->header.type_info = &chaos_mt_System_Private_CoreLib_System_String.hot;
	chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_view.byte_count);
	auto* owned_utf8 = static_cast<char*>(chaos::il2cpp::runtime_core::GcAllocateAtomic(chaos_view.byte_count + 1));
	CHAOS_IL2CPP_MEMCPY(owned_utf8, chaos_view.utf8_data, chaos_view.byte_count);
	owned_utf8[chaos_view.byte_count] = '\0';
	chaos_string->utf8_data = owned_utf8;
	chaos_string->string_id = chaos_id;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
}


	// AOT-baked string table: sorted by StringId for binary search at runtime.
	constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
		{ 155200803463458959U, "RTS_IFACE_RECT_AREA=", 20u },
		{ 1496686715158347433U, "Circle", 6u },
		{ 1932221482344423325U, "RTS_IFACE_COLOR=", 16u },
		{ 1962494976064263795U, "RTS_VIRT_CIRCLE=", 16u },
		{ 2301865087791274877U, "Red", 3u },
		{ 3308291604520544167U, "Shape", 5u },
		{ 4451066142940268809U, "RTS_VIRT_RECT=", 14u },
		{ 6505840729063364189U, "RTS_IFACE_GETNAME=", 18u },
		{ 6704562279895895087U, "Rectangle", 9u },
		{ 6764446324755329143U, "RTS_DONE=ok", 11u },
		{ 7544444272264321929U, "RTS_IFACE_CIRCLE_AREA=", 22u },
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

static void chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	if (chaos_arg_0 != 0) {
		const char* utf8 = nullptr;
		CHAOS_IL2CPP_UINT32 byte_count = 0;
		if (chaos_is_string_id(chaos_arg_0)) {
			auto sv = chaos::il2cpp::string_table::Resolve(chaos_extract_string_id(chaos_arg_0));
			if (sv.utf8_data != nullptr) {
				utf8 = sv.utf8_data;
				byte_count = sv.byte_count;
			}
		} else {
			auto* hdr = reinterpret_cast<const ::StubStringHeader*>(chaos_arg_0);
			utf8 = ::stub_string_data(reinterpret_cast<const void*>(chaos_arg_0));
			byte_count = static_cast<CHAOS_IL2CPP_UINT32>(hdr->byte_count);
		}
		if (utf8 != nullptr && byte_count > 0) {
			std::fwrite(utf8, 1, byte_count, stdout);
		}
		std::fputc('\n', stdout);
		std::fflush(stdout);
	}
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(CHAOS_IL2CPP_INTPTR chaos_arg_0)
{
	auto* chaos_value_slot = chaos_resolve_native_int_slot(chaos_arg_0);
	const CHAOS_IL2CPP_STRING chaos_formatted = ChaosIl2cpp::Common::format_int32(static_cast<CHAOS_IL2CPP_INT32>(*chaos_value_slot));
	const auto chaos_id = chaos::il2cpp::string_table::Intern(
		chaos_formatted.c_str(), static_cast<CHAOS_IL2CPP_UINT32>(chaos_formatted.size()));
	return chaos_make_string_id_value(chaos_id);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	return ChaosReflectionConcatStringPairValues(chaos_fn_arg_0, chaos_fn_arg_1);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void VirtualDispatch_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void VirtualDispatch_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void VirtualDispatch_Circle__ctor_System_Double(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, double chaos_fn_arg_1);
extern "C" double VirtualDispatch_Circle_Area(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR VirtualDispatch_Circle_GetColor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR VirtualDispatch_Circle_GetName(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR VirtualDispatch_DispatchSubjects_TestDerivedInterfaceCall_IColoredShape(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" double VirtualDispatch_DispatchSubjects_TestInterfaceCall_IShape(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR VirtualDispatch_DispatchSubjects_TestVirtualCall_Shape(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void VirtualDispatch_Program__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void VirtualDispatch_Program_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void VirtualDispatch_Rectangle__ctor_System_Double_System_Double(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, double chaos_fn_arg_1, double chaos_fn_arg_2);
extern "C" double VirtualDispatch_Rectangle_Area(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR VirtualDispatch_Rectangle_GetName(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void VirtualDispatch_Shape__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR VirtualDispatch_Shape_GetName(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[17] = {
	0u,
	0u,
	1u,
	2u,
	2u,
	2u,
	2u,
	3u,
	4u,
	5u,
	5u,
	6u,
	8u,
	8u,
	8u,
	8u,
	8u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[16];
	CHAOS_IL2CPP_UINT8 params[8];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		16u,
		8u,
		2386071221u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // VirtualDispatch_AutoGeneratedProgram__ctor
		{ 0u, 1u },  // VirtualDispatch_AutoGeneratedProgram_Main_System_String
		{ 0u, 1u },  // VirtualDispatch_Circle__ctor_System_Double
		{ 9u, 0u },  // VirtualDispatch_Circle_Area
		{ 2u, 0u },  // VirtualDispatch_Circle_GetColor
		{ 2u, 0u },  // VirtualDispatch_Circle_GetName
		{ 2u, 1u },  // VirtualDispatch_DispatchSubjects_TestDerivedInterfaceCall_IColoredShape
		{ 9u, 1u },  // VirtualDispatch_DispatchSubjects_TestInterfaceCall_IShape
		{ 2u, 1u },  // VirtualDispatch_DispatchSubjects_TestVirtualCall_Shape
		{ 0u, 0u },  // VirtualDispatch_Program__ctor
		{ 0u, 1u },  // VirtualDispatch_Program_Main_System_String
		{ 0u, 2u },  // VirtualDispatch_Rectangle__ctor_System_Double_System_Double
		{ 9u, 0u },  // VirtualDispatch_Rectangle_Area
		{ 2u, 0u },  // VirtualDispatch_Rectangle_GetName
		{ 0u, 0u },  // VirtualDispatch_Shape__ctor
		{ 2u, 0u },  // VirtualDispatch_Shape_GetName
	},
	{
		2u,
		9u,
		2u,
		2u,
		2u,
		2u,
		9u,
		9u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[9] = {
		0u,
		0u,
		0u,
		1556u,
		1556u,
		1536u,
		1536u,
		1536u,
		1548u,
	};

	static constexpr const char* s_type_names[9] = {
		"<Module>",
		"Program",
		"AutoGeneratedProgram",
		"IShape",
		"IColoredShape",
		"Shape",
		"Circle",
		"Rectangle",
		"DispatchSubjects",
	};

	static constexpr const char* s_type_namespaces[9] = {
		"",
		"",
		"",
		"VirtualDispatch",
		"VirtualDispatch",
		"VirtualDispatch",
		"VirtualDispatch",
		"VirtualDispatch",
		"VirtualDispatch",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[9] = {
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
		33554438u,
		33554438u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[9] = {
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

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[10] = {
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

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[10] = {
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



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "VirtualDispatch",
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
		/* .type_count        = */ 9u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("VirtualDispatch", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[16] = {
	{ ".ctor", 0x0000000Fu, 0u },  // AutoGeneratedProgram
	{ "Main", 0x0000000Eu, 1u },  // AutoGeneratedProgram
	{ ".ctor", 0x00000015u, 1u },  // Circle
	{ "Area", 0x00000017u, 0u },  // Circle
	{ "GetColor", 0x00000018u, 0u },  // Circle
	{ "GetName", 0x00000016u, 0u },  // Circle
	{ "TestDerivedInterfaceCall", 0x0000001Eu, 1u },  // DispatchSubjects
	{ "TestInterfaceCall", 0x0000001Du, 1u },  // DispatchSubjects
	{ "TestVirtualCall", 0x0000001Cu, 1u },  // DispatchSubjects
	{ ".ctor", 0x0000000Du, 0u },  // Program
	{ "<Main>$", 0x0000000Cu, 1u },  // Program
	{ ".ctor", 0x00000019u, 2u },  // Rectangle
	{ "Area", 0x0000001Bu, 0u },  // Rectangle
	{ "GetName", 0x0000001Au, 0u },  // Rectangle
	{ ".ctor", 0x00000014u, 0u },  // Shape
	{ "GetName", 0x00000013u, 0u },  // Shape
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[6] = {
	{ "AutoGeneratedProgram", "", 0u, 2u },
	{ "Circle", "", 2u, 4u },
	{ "DispatchSubjects", "", 6u, 3u },
	{ "Program", "", 9u, 2u },
	{ "Rectangle", "", 11u, 3u },
	{ "Shape", "", 14u, 2u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[16] = {
	{ 0x0000000Cu, 10u },
	{ 0x0000000Du, 9u },
	{ 0x0000000Eu, 1u },
	{ 0x0000000Fu, 0u },
	{ 0x00000013u, 15u },
	{ 0x00000014u, 14u },
	{ 0x00000015u, 2u },
	{ 0x00000016u, 5u },
	{ 0x00000017u, 3u },
	{ 0x00000018u, 4u },
	{ 0x00000019u, 11u },
	{ 0x0000001Au, 13u },
	{ 0x0000001Bu, 12u },
	{ 0x0000001Cu, 8u },
	{ 0x0000001Du, 7u },
	{ 0x0000001Eu, 6u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[16] = {
	{ reinterpret_cast<void*>(&VirtualDispatch_AutoGeneratedProgram__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // AutoGeneratedProgram::.ctor
	{ reinterpret_cast<void*>(&VirtualDispatch_AutoGeneratedProgram_Main_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // AutoGeneratedProgram::Main
	{ reinterpret_cast<void*>(&VirtualDispatch_Circle__ctor_System_Double), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // Circle::.ctor
	{ reinterpret_cast<void*>(&VirtualDispatch_Circle_Area), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // Circle::Area
	{ reinterpret_cast<void*>(&VirtualDispatch_Circle_GetColor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // Circle::GetColor
	{ reinterpret_cast<void*>(&VirtualDispatch_Circle_GetName), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // Circle::GetName
	{ reinterpret_cast<void*>(&VirtualDispatch_DispatchSubjects_TestDerivedInterfaceCall_IColoredShape), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // DispatchSubjects::TestDerivedInterfaceCall
	{ reinterpret_cast<void*>(&VirtualDispatch_DispatchSubjects_TestInterfaceCall_IShape), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // DispatchSubjects::TestInterfaceCall
	{ reinterpret_cast<void*>(&VirtualDispatch_DispatchSubjects_TestVirtualCall_Shape), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // DispatchSubjects::TestVirtualCall
	{ reinterpret_cast<void*>(&VirtualDispatch_Program__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // Program::.ctor
	{ reinterpret_cast<void*>(&VirtualDispatch_Program_Main_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // Program::<Main>$
	{ reinterpret_cast<void*>(&VirtualDispatch_Rectangle__ctor_System_Double_System_Double), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // Rectangle::.ctor
	{ reinterpret_cast<void*>(&VirtualDispatch_Rectangle_Area), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // Rectangle::Area
	{ reinterpret_cast<void*>(&VirtualDispatch_Rectangle_GetName), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // Rectangle::GetName
	{ reinterpret_cast<void*>(&VirtualDispatch_Shape__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // Shape::.ctor
	{ reinterpret_cast<void*>(&VirtualDispatch_Shape_GetName), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // Shape::GetName
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"VirtualDispatch",
	s_hotpatch_types,
	6u,
	s_hotpatch_methods,
	16u,
	s_hotpatch_slots,
	16u,
	s_hotpatch_entries,
	16u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[7] = {
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
	"VirtualDispatch/Circle::_radius",
	"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)",
	"System.Console/System.Console::WriteLine:System.Void(System.String)",
	"System.Private.CoreLib/System.Int32::ToString:System.String()",
	"VirtualDispatch/Rectangle::_w",
	"VirtualDispatch/Rectangle::_h",
};

extern "C" void* kChaosExternalRuntimeFnTable[7] = {
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__),
	nullptr,
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 7;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[16])() = {
	reinterpret_cast<void(*)()>(&VirtualDispatch_AutoGeneratedProgram__ctor),
	reinterpret_cast<void(*)()>(&VirtualDispatch_AutoGeneratedProgram_Main_System_String),
	reinterpret_cast<void(*)()>(&VirtualDispatch_Circle__ctor_System_Double),
	reinterpret_cast<void(*)()>(&VirtualDispatch_Circle_Area),
	reinterpret_cast<void(*)()>(&VirtualDispatch_Circle_GetColor),
	reinterpret_cast<void(*)()>(&VirtualDispatch_Circle_GetName),
	reinterpret_cast<void(*)()>(&VirtualDispatch_DispatchSubjects_TestDerivedInterfaceCall_IColoredShape),
	reinterpret_cast<void(*)()>(&VirtualDispatch_DispatchSubjects_TestInterfaceCall_IShape),
	reinterpret_cast<void(*)()>(&VirtualDispatch_DispatchSubjects_TestVirtualCall_Shape),
	reinterpret_cast<void(*)()>(&VirtualDispatch_Program__ctor),
	reinterpret_cast<void(*)()>(&VirtualDispatch_Program_Main_System_String),
	reinterpret_cast<void(*)()>(&VirtualDispatch_Rectangle__ctor_System_Double_System_Double),
	reinterpret_cast<void(*)()>(&VirtualDispatch_Rectangle_Area),
	reinterpret_cast<void(*)()>(&VirtualDispatch_Rectangle_GetName),
	reinterpret_cast<void(*)()>(&VirtualDispatch_Shape__ctor),
	reinterpret_cast<void(*)()>(&VirtualDispatch_Shape_GetName),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[16])() = {
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[0])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[1])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[2])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[3])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[4])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[5])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[6])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[7])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[8])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[9])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[10])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[11])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[12])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[13])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[14])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[15])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
};


// Single-method dispatch via hotpatch dispatch table.
// NOTE: Uses kBenchmarkWrappers (not raw kAotMethods) to ensure instance
// methods receive a valid this-pointer sentinel and default argument values.
extern "C" CHAOS_IL2CPP_INT32 RunNativeAot(
	CHAOS_IL2CPP_INT32 chaos_entry_index)
{
	if (chaos_entry_index < 0 || chaos_entry_index >= kAotMethodCount)
		return -1;
	auto& entry = s_hotpatch_entries[chaos_entry_index];
	if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry)
		&& !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry))
	{
		uint64_t __chaos_args[4] = {}; uint64_t __chaos_ret[2] = {};
		chaos::il2cpp::runtime_core::InterpreterEntryDirect(
			entry.method_key, __chaos_args, __chaos_ret);
	} else {
		kBenchmarkWrappers[chaos_entry_index]();
	}
	return 0;
}

// All-methods loop: run every method and return a bitmask of failures.
extern "C" CHAOS_IL2CPP_INT32 RunNativeAotAll()
{
	CHAOS_IL2CPP_INT32 result = 0;
	for (int i = 0; i < kAotMethodCount; i++) {
		auto& entry = s_hotpatch_entries[i];
		if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry)
			&& !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry))
		{
			uint64_t __chaos_args[4] = {}; uint64_t __chaos_ret[2] = {};
			chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				entry.method_key, __chaos_args, __chaos_ret);
		} else {
			// Use kBenchmarkWrappers which supply correct default argument values
			kBenchmarkWrappers[i]();
		}
	}
	return result;
}

// Fast benchmark dispatch: no setjmp, inline slot access.
extern "C" CHAOS_IL2CPP_INT32 RunNativeAotBench(
	CHAOS_IL2CPP_INT32 chaos_entry_index)
{
	if (chaos_entry_index < 0 || chaos_entry_index >= kAotMethodCount)
		return -1;
	auto& entry = s_hotpatch_entries[chaos_entry_index];
	if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry)
		&& !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry))
	{
		chaos::il2cpp::runtime_core::InterpreterEntryDirectFast(
			entry.method_key);
	} else {
		kBenchmarkWrappers[chaos_entry_index]();
	}
	return 0;
}

// Pure AOT benchmark: calls kBenchmarkWrappers[i] directly, no hotpatch overhead.
extern "C" double BenchmarkMethod(
	int chaos_entry_index, int iterations) {
	if (chaos_entry_index < 0 || chaos_entry_index >= kAotMethodCount)
		return -1.0;
	auto start = std::chrono::steady_clock::now();
	for (int i = 0; i < iterations; i++) {
		kBenchmarkWrappers[chaos_entry_index]();
	}
	auto end = std::chrono::steady_clock::now();
	return std::chrono::duration<double, std::milli>(
		end - start).count();
}
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

	/* ── Entry 0: VirtualDispatch_AutoGeneratedProgram__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry0;
	/* ── Entry 1: VirtualDispatch_AutoGeneratedProgram_Main_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry1;
	/* ── Entry 2: VirtualDispatch_Circle__ctor_System_Double ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: VirtualDispatch_Circle_Area ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry3;
	/* ── Entry 4: VirtualDispatch_Circle_GetColor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry4;
	/* ── Entry 5: VirtualDispatch_Circle_GetName ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry5;
	/* ── Entry 6: VirtualDispatch_DispatchSubjects_TestDerivedInterfaceCall_IColoredShape ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry6;
	/* ── Entry 7: VirtualDispatch_DispatchSubjects_TestInterfaceCall_IShape ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry7;
	/* ── Entry 8: VirtualDispatch_DispatchSubjects_TestVirtualCall_Shape ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry8;
	/* ── Entry 9: VirtualDispatch_Program__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry9;
	/* ── Entry 10: VirtualDispatch_Program_Main_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[10];
	} entry10;
	/* ── Entry 11: VirtualDispatch_Rectangle__ctor_System_Double_System_Double ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry11;
	/* ── Entry 12: VirtualDispatch_Rectangle_Area ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry12;
	/* ── Entry 13: VirtualDispatch_Rectangle_GetName ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry13;
	/* ── Entry 14: VirtualDispatch_Shape__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry14;
	/* ── Entry 15: VirtualDispatch_Shape_GetName ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry15;
} kChaosGcSlotMapsSection = {
	/* entry0 = VirtualDispatch_AutoGeneratedProgram__ctor */
	.entry0 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&VirtualDispatch_AutoGeneratedProgram__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry1 = VirtualDispatch_AutoGeneratedProgram_Main_System_String */
	.entry1 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&VirtualDispatch_AutoGeneratedProgram_Main_System_String),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry2 = VirtualDispatch_Circle__ctor_System_Double */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&VirtualDispatch_Circle__ctor_System_Double),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 16u, 24u }
	},
	/* entry3 = VirtualDispatch_Circle_Area */
	.entry3 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&VirtualDispatch_Circle_Area),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry4 = VirtualDispatch_Circle_GetColor */
	.entry4 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&VirtualDispatch_Circle_GetColor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry5 = VirtualDispatch_Circle_GetName */
	.entry5 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&VirtualDispatch_Circle_GetName),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry6 = VirtualDispatch_DispatchSubjects_TestDerivedInterfaceCall_IColoredShape */
	.entry6 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&VirtualDispatch_DispatchSubjects_TestDerivedInterfaceCall_IColoredShape),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry7 = VirtualDispatch_DispatchSubjects_TestInterfaceCall_IShape */
	.entry7 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&VirtualDispatch_DispatchSubjects_TestInterfaceCall_IShape),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry8 = VirtualDispatch_DispatchSubjects_TestVirtualCall_Shape */
	.entry8 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&VirtualDispatch_DispatchSubjects_TestVirtualCall_Shape),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry9 = VirtualDispatch_Program__ctor */
	.entry9 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&VirtualDispatch_Program__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry10 = VirtualDispatch_Program_Main_System_String */
	.entry10 = {
		/* entry_total_size = 60 */ 60u,
		/* code_address */ reinterpret_cast<const void*>(&VirtualDispatch_Program_Main_System_String),
		/* frame_size = 80 */ 80u,
		/* num_gc_slots = 10 */ 10u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u }
	},
	/* entry11 = VirtualDispatch_Rectangle__ctor_System_Double_System_Double */
	.entry11 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&VirtualDispatch_Rectangle__ctor_System_Double_System_Double),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 24u, 32u }
	},
	/* entry12 = VirtualDispatch_Rectangle_Area */
	.entry12 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&VirtualDispatch_Rectangle_Area),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry13 = VirtualDispatch_Rectangle_GetName */
	.entry13 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&VirtualDispatch_Rectangle_GetName),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry14 = VirtualDispatch_Shape__ctor */
	.entry14 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&VirtualDispatch_Shape__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry15 = VirtualDispatch_Shape_GetName */
	.entry15 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&VirtualDispatch_Shape_GetName),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 508u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[16] = {
	reinterpret_cast<void*>(&VirtualDispatch_AutoGeneratedProgram__ctor),
	reinterpret_cast<void*>(&VirtualDispatch_AutoGeneratedProgram_Main_System_String),
	reinterpret_cast<void*>(&VirtualDispatch_Circle__ctor_System_Double),
	reinterpret_cast<void*>(&VirtualDispatch_Circle_Area),
	reinterpret_cast<void*>(&VirtualDispatch_Circle_GetColor),
	reinterpret_cast<void*>(&VirtualDispatch_Circle_GetName),
	reinterpret_cast<void*>(&VirtualDispatch_DispatchSubjects_TestDerivedInterfaceCall_IColoredShape),
	reinterpret_cast<void*>(&VirtualDispatch_DispatchSubjects_TestInterfaceCall_IShape),
	reinterpret_cast<void*>(&VirtualDispatch_DispatchSubjects_TestVirtualCall_Shape),
	reinterpret_cast<void*>(&VirtualDispatch_Program__ctor),
	reinterpret_cast<void*>(&VirtualDispatch_Program_Main_System_String),
	reinterpret_cast<void*>(&VirtualDispatch_Rectangle__ctor_System_Double_System_Double),
	reinterpret_cast<void*>(&VirtualDispatch_Rectangle_Area),
	reinterpret_cast<void*>(&VirtualDispatch_Rectangle_GetName),
	reinterpret_cast<void*>(&VirtualDispatch_Shape__ctor),
	reinterpret_cast<void*>(&VirtualDispatch_Shape_GetName),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0x75431216481870AC),
		0x00000005u,
		0u,
		2u,
		reinterpret_cast<const void*>(kSlots_VirtualDispatch_Shape),
		reinterpret_cast<const void**>(chaos_vtable_VirtualDispatch_Shape),
		2u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x941A587BB5C1B3EB),
		0x00000006u,
		0x00000005u,
		5u,
		reinterpret_cast<const void*>(kSlots_VirtualDispatch_Circle),
		reinterpret_cast<const void**>(chaos_vtable_VirtualDispatch_Circle),
		5u,
		1,
		{0, 0, 0},
		chaos_iface_map_VirtualDispatch_Circle,
		2u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x93333F1C47D59F28),
		0x00000007u,
		0x00000005u,
		6u,
		reinterpret_cast<const void*>(kSlots_VirtualDispatch_Rectangle),
		reinterpret_cast<const void**>(chaos_vtable_VirtualDispatch_Rectangle),
		6u,
		1,
		{0, 0, 0},
		chaos_iface_map_VirtualDispatch_Rectangle,
		1u,
	},
};
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 16u,
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
	.image_name_utf8    = "VirtualDispatch",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryMethodDescriptor kReflMethods_VirtualDispatch_AutoGeneratedProgram[2] = {
	{ 0u, "VirtualDispatch/AutoGeneratedProgram::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "VirtualDispatch/AutoGeneratedProgram::Main:System.Void(System.String[])", "Main", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_VirtualDispatch_Circle[4] = {
	{ 0u, "VirtualDispatch/Circle::.ctor:System.Void(System.Double)", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "VirtualDispatch/Circle::Area:System.Double()", "Area", "System.Void", 0, nullptr, 0u },
	{ 0u, "VirtualDispatch/Circle::GetColor:System.String()", "GetColor", "System.Void", 0, nullptr, 0u },
	{ 0u, "VirtualDispatch/Circle::GetName:System.String()", "GetName", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_VirtualDispatch_DispatchSubjects[3] = {
	{ 0u, "VirtualDispatch/DispatchSubjects::TestDerivedInterfaceCall:System.String(IColoredShape)", "TestDerivedInterfaceCall", "System.Void", 0, nullptr, 0u },
	{ 0u, "VirtualDispatch/DispatchSubjects::TestInterfaceCall:System.Double(IShape)", "TestInterfaceCall", "System.Void", 0, nullptr, 0u },
	{ 0u, "VirtualDispatch/DispatchSubjects::TestVirtualCall:System.String(Shape)", "TestVirtualCall", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_VirtualDispatch_Program[2] = {
	{ 0u, "VirtualDispatch/Program::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "VirtualDispatch/Program::<Main>$:System.Void(System.String[])", "<Main>$", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_VirtualDispatch_Rectangle[3] = {
	{ 0u, "VirtualDispatch/Rectangle::.ctor:System.Void(System.Double,System.Double)", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "VirtualDispatch/Rectangle::Area:System.Double()", "Area", "System.Void", 0, nullptr, 0u },
	{ 0u, "VirtualDispatch/Rectangle::GetName:System.String()", "GetName", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_VirtualDispatch_Shape[2] = {
	{ 0u, "VirtualDispatch/Shape::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "VirtualDispatch/Shape::GetName:System.String()", "GetName", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[6] = {
	{ 0u, "VirtualDispatch/AutoGeneratedProgram", "VirtualDispatch/AutoGeneratedProgram", "", "AutoGeneratedProgram", "AutoGeneratedProgram", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_VirtualDispatch_AutoGeneratedProgram, 2u },
	{ 0u, "VirtualDispatch/Circle", "VirtualDispatch/Circle", "", "Circle", "Circle", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_VirtualDispatch_Circle, 4u },
	{ 0u, "VirtualDispatch/DispatchSubjects", "VirtualDispatch/DispatchSubjects", "", "DispatchSubjects", "DispatchSubjects", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_VirtualDispatch_DispatchSubjects, 3u },
	{ 0u, "VirtualDispatch/Program", "VirtualDispatch/Program", "", "Program", "Program", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_VirtualDispatch_Program, 2u },
	{ 0u, "VirtualDispatch/Rectangle", "VirtualDispatch/Rectangle", "", "Rectangle", "Rectangle", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_VirtualDispatch_Rectangle, 3u },
	{ 0u, "VirtualDispatch/Shape", "VirtualDispatch/Shape", "", "Shape", "Shape", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_VirtualDispatch_Shape, 2u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[6] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
	&kReflTypes[3],
	&kReflTypes[4],
	&kReflTypes[5],
};

static const ReflectionQueryImageDescriptor kReflImage = { "VirtualDispatch", kReflTypePtrs, 6u };

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
// Managed method: VirtualDispatch/AutoGeneratedProgram::.ctor()
extern "C" void VirtualDispatch_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: VirtualDispatch/AutoGeneratedProgram::Main(System.String[])
extern "C" void VirtualDispatch_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: VirtualDispatch/Circle::.ctor(System.Double)
extern "C" void VirtualDispatch_Circle__ctor_System_Double(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, double chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = ChaosStoreFloat64(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s0);
		auto& _d14 = s_hotpatch_entries[14];
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d14)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d14))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d14.method_key, _d_ab, nullptr);
		}
		else
		{
			VirtualDispatch_Shape__ctor(chaos_arg_0);
		}
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_VirtualDispatch_Circle*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_VirtualDispatch_Circle___radius));
		chaos_object->field_VirtualDispatch_Circle___radius = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: VirtualDispatch/Circle::Area()
extern "C" double VirtualDispatch_Circle_Area(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = ChaosStoreFloat64(3.14159);
	_s1 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_VirtualDispatch_Circle*>(_s1);
		_s1 = chaos_object->field_VirtualDispatch_Circle___radius;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), ChaosLoadFloat64(_s1)));
	_s1 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_VirtualDispatch_Circle*>(_s1);
		_s1 = chaos_object->field_VirtualDispatch_Circle___radius;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return ChaosLoadFloat64(_s0);
}

// Managed method: VirtualDispatch/Circle::GetColor()
extern "C" CHAOS_IL2CPP_INTPTR VirtualDispatch_Circle_GetColor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("Red");
	}}
	return _s0;
}

// Managed method: VirtualDispatch/Circle::GetName()
extern "C" CHAOS_IL2CPP_INTPTR VirtualDispatch_Circle_GetName(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("Circle");
	}}
	return _s0;
}

// Managed method: VirtualDispatch/DispatchSubjects::TestDerivedInterfaceCall(IColoredShape)
extern "C" CHAOS_IL2CPP_INTPTR VirtualDispatch_DispatchSubjects_TestDerivedInterfaceCall_IColoredShape(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INTPTR chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 4u)))(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	return _s0;
}

// Managed method: VirtualDispatch/DispatchSubjects::TestInterfaceCall(IShape)
extern "C" double VirtualDispatch_DispatchSubjects_TestInterfaceCall_IShape(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		double chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<double(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 3u)))(chaos_arg_0);
		_s0 = ChaosStoreFloat64(chaos_callvirt_result);
	}
	return ChaosLoadFloat64(_s0);
}

// Managed method: VirtualDispatch/DispatchSubjects::TestVirtualCall(Shape)
extern "C" CHAOS_IL2CPP_INTPTR VirtualDispatch_DispatchSubjects_TestVirtualCall_Shape(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INTPTR chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 1u)))(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	return _s0;
}

// Managed method: VirtualDispatch/Program::.ctor()
extern "C" void VirtualDispatch_Program__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: VirtualDispatch/Program::<Main>$(System.String[])
extern "C" void VirtualDispatch_Program_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 5) chaos_locals{};
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
	CHAOS_IL2CPP_INTPTR _s19{};
	CHAOS_IL2CPP_INTPTR _s20{};
	CHAOS_IL2CPP_INTPTR _s21{};
	CHAOS_IL2CPP_INTPTR _s22{};
	CHAOS_IL2CPP_INTPTR _s23{};
	CHAOS_IL2CPP_INTPTR _s24{};
	CHAOS_IL2CPP_INTPTR _s25{};
	CHAOS_IL2CPP_INTPTR _s26{};
	CHAOS_IL2CPP_INTPTR _s27{};
	CHAOS_IL2CPP_INTPTR _s28{};
	CHAOS_IL2CPP_INTPTR _s29{};
	CHAOS_IL2CPP_INTPTR _s30{};
	CHAOS_IL2CPP_INTPTR _s31{};
	CHAOS_IL2CPP_INTPTR _s32{};
	CHAOS_IL2CPP_INTPTR _s33{};
	CHAOS_IL2CPP_INTPTR _s34{};
	CHAOS_IL2CPP_INTPTR _s35{};
	CHAOS_IL2CPP_INTPTR _s36{};
	CHAOS_IL2CPP_INTPTR _s37{};
	CHAOS_IL2CPP_INTPTR _s38{};
	CHAOS_IL2CPP_INTPTR _s39{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = ChaosStoreFloat64(5);
	{
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_VirtualDispatch_Circle, {});
		chaos_object->header.type_info = &chaos_mt_VirtualDispatch_Circle.hot;
		VirtualDispatch_Circle__ctor_System_Double(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), ChaosLoadFloat64(chaos_arg_1));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = ChaosStoreFloat64(3);
	_s1 = ChaosStoreFloat64(4);
	{
		const auto chaos_arg_2 = _s1;
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_VirtualDispatch_Rectangle, {});
		chaos_object->header.type_info = &chaos_mt_VirtualDispatch_Rectangle.hot;
		VirtualDispatch_Rectangle__ctor_System_Double_System_Double(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), ChaosLoadFloat64(chaos_arg_1), ChaosLoadFloat64(chaos_arg_2));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[1] = _s0;
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_VIRT_CIRCLE=");
	}}
	_s1 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		auto& _d8 = s_hotpatch_entries[8];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d8)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d8))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d8.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = VirtualDispatch_DispatchSubjects_TestVirtualCall_Shape(chaos_arg_0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_VIRT_RECT=");
	}}
	_s1 = chaos_locals[1];
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s1);
		auto& _d8 = s_hotpatch_entries[8];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d8)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d8))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d8.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = VirtualDispatch_DispatchSubjects_TestVirtualCall_Shape(chaos_arg_0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s0;
		auto& _d7 = s_hotpatch_entries[7];
		double _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d7.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = VirtualDispatch_DispatchSubjects_TestInterfaceCall_IShape(chaos_arg_0);
		}
		_s0 = ChaosStoreFloat64(_d_hpresult);
	}
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[1];
	{
		const auto chaos_arg_0 = _s0;
		auto& _d7 = s_hotpatch_entries[7];
		double _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d7.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = VirtualDispatch_DispatchSubjects_TestInterfaceCall_IShape(chaos_arg_0);
		}
		_s0 = ChaosStoreFloat64(_d_hpresult);
	}
	chaos_locals[3] = _s0;
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_IFACE_CIRCLE_AREA=");
	}}
	_s1 = chaos_locals[2];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(ChaosLoadFloat64(_s1)));
	chaos_locals[4] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[4]);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_IFACE_RECT_AREA=");
	}}
	_s1 = chaos_locals[3];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
	chaos_locals[4] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[4]);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_IFACE_COLOR=");
	}}
	_s1 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s1;
		auto& _d6 = s_hotpatch_entries[6];
		CHAOS_IL2CPP_INTPTR _d_hpresult{};
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d6.method_key, _d_ab, &_d_hpresult);
		}
		else
		{
			_d_hpresult = VirtualDispatch_DispatchSubjects_TestDerivedInterfaceCall_IColoredShape(chaos_arg_0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_IFACE_GETNAME=");
	}}
	_s1 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		CHAOS_IL2CPP_INTPTR chaos_callvirt_result{};
		chaos_callvirt_result = (*reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)>(chaos_vtable_resolve(chaos_object_get_type_info(reinterpret_cast<void*>(chaos_arg_0))->vtable_array, 1u)))(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_callvirt_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_DONE=ok");
	}}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	return;
}

// Managed method: VirtualDispatch/Rectangle::.ctor(System.Double,System.Double)
extern "C" void VirtualDispatch_Rectangle__ctor_System_Double_System_Double(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, double chaos_fn_arg_1, double chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = ChaosStoreFloat64(chaos_fn_arg_1);
	chaos_args[2] = ChaosStoreFloat64(chaos_fn_arg_2);

	_s0 = chaos_args[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s0);
		auto& _d14 = s_hotpatch_entries[14];
		if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d14)
			&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d14))
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d14.method_key, _d_ab, nullptr);
		}
		else
		{
			VirtualDispatch_Shape__ctor(chaos_arg_0);
		}
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_VirtualDispatch_Rectangle*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_VirtualDispatch_Rectangle___w));
		chaos_object->field_VirtualDispatch_Rectangle___w = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	_s0 = chaos_args[0];
	_s1 = chaos_args[2];
	{
		auto chaos_value = _s1;
		if (chaos_is_string_id(chaos_value))
		{
			chaos_value = chaos_string_materialize(chaos_value);
		}
		auto* chaos_object = reinterpret_cast<chaos_type_VirtualDispatch_Rectangle*>(_s0);
		BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_object->field_VirtualDispatch_Rectangle___h));
		chaos_object->field_VirtualDispatch_Rectangle___h = chaos_value;
		chaos_gc_dirty_card(chaos_object);
	}
	return;
}

// Managed method: VirtualDispatch/Rectangle::Area()
extern "C" double VirtualDispatch_Rectangle_Area(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_VirtualDispatch_Rectangle*>(_s0);
		_s0 = chaos_object->field_VirtualDispatch_Rectangle___w;
	}
	_s1 = chaos_args[0];
	{
		auto* chaos_object = reinterpret_cast<chaos_type_VirtualDispatch_Rectangle*>(_s1);
		_s1 = chaos_object->field_VirtualDispatch_Rectangle___h;
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return ChaosLoadFloat64(_s0);
}

// Managed method: VirtualDispatch/Rectangle::GetName()
extern "C" CHAOS_IL2CPP_INTPTR VirtualDispatch_Rectangle_GetName(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("Rectangle");
	}}
	return _s0;
}

// Managed method: VirtualDispatch/Shape::.ctor()
extern "C" void VirtualDispatch_Shape__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: VirtualDispatch/Shape::GetName()
extern "C" CHAOS_IL2CPP_INTPTR VirtualDispatch_Shape_GetName(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("Shape");
	}}
	return _s0;
}



}  // namespace chaos::il2cpp::codegen::VirtualDispatch
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 16;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"virtual-dispatch.auto-generated-program.ctor\",\"subjectId\":\"VirtualDispatch/AutoGeneratedProgram::.ctor:System.Void()\",\"signature\":\"System.Void AutoGeneratedProgram::.ctor()\",\"identity\":{\"assemblyName\":\"VirtualDispatch\",\"declaringTypeSubjectId\":\"VirtualDispatch/AutoGeneratedProgram\",\"definitionSubjectId\":\"VirtualDispatch/AutoGeneratedProgram::.ctor:System.Void()\",\"subjectId\":\"VirtualDispatch/AutoGeneratedProgram::.ctor:System.Void()\",\"methodId\":\"virtual-dispatch.auto-generated-program.ctor\",\"signature\":\"System.Void AutoGeneratedProgram::.ctor()\"},\"nativeSymbol\":\"VirtualDispatch_AutoGeneratedProgram__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"VirtualDispatch/AutoGeneratedProgram\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"VirtualDispatch\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"virtual-dispatch.auto-generated-program.main\",\"subjectId\":\"VirtualDispatch/AutoGeneratedProgram::Main:System.Void(System.String[])\",\"signature\":\"System.Void AutoGeneratedProgram::Main(System.String[])\",\"identity\":{\"assemblyName\":\"VirtualDispatch\",\"declaringTypeSubjectId\":\"VirtualDispatch/AutoGeneratedProgram\",\"definitionSubjectId\":\"VirtualDispatch/AutoGeneratedProgram::Main:System.Void(System.String[])\",\"subjectId\":\"VirtualDispatch/AutoGeneratedProgram::Main:System.Void(System.String[])\",\"methodId\":\"virtual-dispatch.auto-generated-program.main\",\"signature\":\"System.Void AutoGeneratedProgram::Main(System.String[])\"},\"nativeSymbol\":\"VirtualDispatch_AutoGeneratedProgram_Main_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":1}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":0,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"VirtualDispatch\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"virtual-dispatch.circle.ctor\",\"subjectId\":\"VirtualDispatch/Circle::.ctor:System.Void(System.Double)\",\"signature\":\"System.Void Circle::.ctor(System.Double)\",\"identity\":{\"assemblyName\":\"VirtualDispatch\",\"declaringTypeSubjectId\":\"VirtualDispatch/Circle\",\"definitionSubjectId\":\"VirtualDispatch/Circle::.ctor:System.Void(System.Double)\",\"subjectId\":\"VirtualDispatch/Circle::.ctor:System.Void(System.Double)\",\"methodId\":\"virtual-dispatch.circle.ctor\",\"signature\":\"System.Void Circle::.ctor(System.Double)\"},\"nativeSymbol\":\"VirtualDispatch_Circle__ctor_System_Double\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":9,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"VirtualDispatch/Circle\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"VirtualDispatch/Shape::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"method\",\"subjectId\":\"VirtualDispatch/Shape::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/Shape::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"VirtualDispatch_Shape__ctor\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Void\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":6,\"resultType\":\"VirtualDispatch/Circle\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":7,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"stfld\",\"opCode\":11,\"operand\":\"VirtualDispatch/Circle::_radius\",\"ilOffset\":8,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"field\",\"subjectId\":\"VirtualDispatch/Circle::_radius\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/Circle::_radius\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"VirtualDispatch/Circle\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Double\"},\"runtimeServiceKind\":3,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":13,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"VirtualDispatch\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"virtual-dispatch.circle.area\",\"subjectId\":\"VirtualDispatch/Circle::Area:System.Double()\",\"signature\":\"System.Double Circle::Area()\",\"identity\":{\"assemblyName\":\"VirtualDispatch\",\"declaringTypeSubjectId\":\"VirtualDispatch/Circle\",\"definitionSubjectId\":\"VirtualDispatch/Circle::Area:System.Double()\",\"subjectId\":\"VirtualDispatch/Circle::Area:System.Double()\",\"methodId\":\"virtual-dispatch.circle.area\",\"signature\":\"System.Double Circle::Area()\"},\"nativeSymbol\":\"VirtualDispatch_Circle_Area\",\"isStatic\":false,\"returnType\":\"System.Double\",\"returnAbi\":{\"carrierKindCode\":9,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":3.14159,\"ilOffset\":0,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":9,\"resultType\":\"VirtualDispatch/Circle\",\"isPreserveSig\":false},{\"op\":\"ldfld\",\"opCode\":10,\"operand\":\"VirtualDispatch/Circle::_radius\",\"ilOffset\":10,\"resultType\":\"System.Double\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"field\",\"subjectId\":\"VirtualDispatch/Circle::_radius\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/Circle::_radius\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"VirtualDispatch/Circle\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Double\"},\"runtimeServiceKind\":2,\"isPreserveSig\":false},{\"op\":\"mul\",\"opCode\":27,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":16,\"resultType\":\"VirtualDispatch/Circle\",\"isPreserveSig\":false},{\"op\":\"ldfld\",\"opCode\":10,\"operand\":\"VirtualDispatch/Circle::_radius\",\"ilOffset\":17,\"resultType\":\"System.Double\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"field\",\"subjectId\":\"VirtualDispatch/Circle::_radius\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/Circle::_radius\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"VirtualDispatch/Circle\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Double\"},\"runtimeServiceKind\":2,\"isPreserveSig\":false},{\"op\":\"mul\",\"opCode\":27,\"ilOffset\":22,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":23,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"VirtualDispatch\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"virtual-dispatch.circle.get-color\",\"subjectId\":\"VirtualDispatch/Circle::GetColor:System.String()\",\"signature\":\"System.String Circle::GetColor()\",\"identity\":{\"assemblyName\":\"VirtualDispatch\",\"declaringTypeSubjectId\":\"VirtualDispatch/Circle\",\"definitionSubjectId\":\"VirtualDispatch/Circle::GetColor:System.String()\",\"subjectId\":\"VirtualDispatch/Circle::GetColor:System.String()\",\"methodId\":\"virtual-dispatch.circle.get-color\",\"signature\":\"System.String Circle::GetColor()\"},\"nativeSymbol\":\"VirtualDispatch_Circle_GetColor\",\"isStatic\":false,\"returnType\":\"System.String\",\"returnAbi\":{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"Red\",\"ilOffset\":0,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"VirtualDispatch\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"virtual-dispatch.circle.get-name\",\"subjectId\":\"VirtualDispatch/Circle::GetName:System.String()\",\"signature\":\"System.String Circle::GetName()\",\"identity\":{\"assemblyName\":\"VirtualDispatch\",\"declaringTypeSubjectId\":\"VirtualDispatch/Circle\",\"definitionSubjectId\":\"VirtualDispatch/Circle::GetName:System.String()\",\"subjectId\":\"VirtualDispatch/Circle::GetName:System.String()\",\"methodId\":\"virtual-dispatch.circle.get-name\",\"signature\":\"System.String Circle::GetName()\"},\"nativeSymbol\":\"VirtualDispatch_Circle_GetName\",\"isStatic\":false,\"returnType\":\"System.String\",\"returnAbi\":{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"Circle\",\"ilOffset\":0,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"VirtualDispatch\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"virtual-dispatch.dispatch-subjects.test-derived-interface-call\",\"subjectId\":\"VirtualDispatch/DispatchSubjects::TestDerivedInterfaceCall:System.String(IColoredShape)\",\"signature\":\"System.String DispatchSubjects::TestDerivedInterfaceCall(IColoredShape)\",\"identity\":{\"assemblyName\":\"VirtualDispatch\",\"declaringTypeSubjectId\":\"VirtualDispatch/DispatchSubjects\",\"definitionSubjectId\":\"VirtualDispatch/DispatchSubjects::TestDerivedInterfaceCall:System.String(IColoredShape)\",\"subjectId\":\"VirtualDispatch/DispatchSubjects::TestDerivedInterfaceCall:System.String(IColoredShape)\",\"methodId\":\"virtual-dispatch.dispatch-subjects.test-derived-interface-call\",\"signature\":\"System.String DispatchSubjects::TestDerivedInterfaceCall(IColoredShape)\"},\"nativeSymbol\":\"VirtualDispatch_DispatchSubjects_TestDerivedInterfaceCall_IColoredShape\",\"isStatic\":true,\"returnType\":\"System.String\",\"returnAbi\":{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeSubjectId\":\"VirtualDispatch/IColoredShape\",\"typeShape\":3}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"IColoredShape\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":1,\"resultType\":\"System.String\",\"callee\":\"VirtualDispatch/IColoredShape::GetColor:System.String()\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"method\",\"subjectId\":\"VirtualDispatch/IColoredShape::GetColor:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/IColoredShape::GetColor:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"VirtualDispatch_IColoredShape_GetColor\",\"targetParameterCount\":0,\"targetReturnType\":\"System.String\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"VirtualDispatch\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"virtual-dispatch.dispatch-subjects.test-interface-call\",\"subjectId\":\"VirtualDispatch/DispatchSubjects::TestInterfaceCall:System.Double(IShape)\",\"signature\":\"System.Double DispatchSubjects::TestInterfaceCall(IShape)\",\"identity\":{\"assemblyName\":\"VirtualDispatch\",\"declaringTypeSubjectId\":\"VirtualDispatch/DispatchSubjects\",\"definitionSubjectId\":\"VirtualDispatch/DispatchSubjects::TestInterfaceCall:System.Double(IShape)\",\"subjectId\":\"VirtualDispatch/DispatchSubjects::TestInterfaceCall:System.Double(IShape)\",\"methodId\":\"virtual-dispatch.dispatch-subjects.test-interface-call\",\"signature\":\"System.Double DispatchSubjects::TestInterfaceCall(IShape)\"},\"nativeSymbol\":\"VirtualDispatch_DispatchSubjects_TestInterfaceCall_IShape\",\"isStatic\":true,\"returnType\":\"System.Double\",\"returnAbi\":{\"carrierKindCode\":9,\"typeShape\":2},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeSubjectId\":\"VirtualDispatch/IShape\",\"typeShape\":3}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"IShape\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":1,\"resultType\":\"System.Double\",\"callee\":\"VirtualDispatch/IShape::Area:System.Double()\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"method\",\"subjectId\":\"VirtualDispatch/IShape::Area:System.Double()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/IShape::Area:System.Double()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"VirtualDispatch_IShape_Area\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Double\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"VirtualDispatch\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"virtual-dispatch.dispatch-subjects.test-virtual-call\",\"subjectId\":\"VirtualDispatch/DispatchSubjects::TestVirtualCall:System.String(Shape)\",\"signature\":\"System.String DispatchSubjects::TestVirtualCall(Shape)\",\"identity\":{\"assemblyName\":\"VirtualDispatch\",\"declaringTypeSubjectId\":\"VirtualDispatch/DispatchSubjects\",\"definitionSubjectId\":\"VirtualDispatch/DispatchSubjects::TestVirtualCall:System.String(Shape)\",\"subjectId\":\"VirtualDispatch/DispatchSubjects::TestVirtualCall:System.String(Shape)\",\"methodId\":\"virtual-dispatch.dispatch-subjects.test-virtual-call\",\"signature\":\"System.String DispatchSubjects::TestVirtualCall(Shape)\"},\"nativeSymbol\":\"VirtualDispatch_DispatchSubjects_TestVirtualCall_Shape\",\"isStatic\":true,\"returnType\":\"System.String\",\"returnAbi\":{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeSubjectId\":\"VirtualDispatch/Shape\",\"typeShape\":1}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"Shape\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":1,\"resultType\":\"System.String\",\"callee\":\"VirtualDispatch/Shape::GetName:System.String()\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"method\",\"subjectId\":\"VirtualDispatch/Shape::GetName:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/Shape::GetName:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"VirtualDispatch_Shape_GetName\",\"targetParameterCount\":0,\"targetReturnType\":\"System.String\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"VirtualDispatch\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"virtual-dispatch.program.ctor\",\"subjectId\":\"VirtualDispatch/Program::.ctor:System.Void()\",\"signature\":\"System.Void Program::.ctor()\",\"identity\":{\"assemblyName\":\"VirtualDispatch\",\"declaringTypeSubjectId\":\"VirtualDispatch/Program\",\"definitionSubjectId\":\"VirtualDispatch/Program::.ctor:System.Void()\",\"subjectId\":\"VirtualDispatch/Program::.ctor:System.Void()\",\"methodId\":\"virtual-dispatch.program.ctor\",\"signature\":\"System.Void Program::.ctor()\"},\"nativeSymbol\":\"VirtualDispatch_Program__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"VirtualDispatch/Program\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"VirtualDispatch\"}";
static const char kMethodJson_10[] = 
    "{\"methodId\":\"virtual-dispatch.program.main\",\"subjectId\":\"VirtualDispatch/Program::\\u003CMain\\u003E$:System.Void(System.String[])\",\"signature\":\"System.Void Program::\\u003CMain\\u003E$(System.String[])\",\"identity\":{\"assemblyName\":\"VirtualDispatch\",\"declaringTypeSubjectId\":\"VirtualDispatch/Program\",\"definitionSubjectId\":\"VirtualDispatch/Program::\\u003CMain\\u003E$:System.Void(System.String[])\",\"subjectId\":\"VirtualDispatch/Program::\\u003CMain\\u003E$:System.Void(System.String[])\",\"methodId\":\"virtual-dispatch.program.main\",\"signature\":\"System.Void Program::\\u003CMain\\u003E$(System.String[])\"},\"nativeSymbol\":\"VirtualDispatch_Program_Main_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":1}],\"localCount\":5,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":5,\"ilOffset\":0,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":9,\"resultType\":\"VirtualDispatch/Circle\",\"callee\":\"VirtualDispatch/Circle::.ctor:System.Void(System.Double)\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"method\",\"subjectId\":\"VirtualDispatch/Circle::.ctor:System.Void(System.Double)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/Circle\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"VirtualDispatch/Shape\",\"implementedInterfaceSubjectIds\":[\"VirtualDispatch/IColoredShape\",\"VirtualDispatch/IShape\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":3,\"ilOffset\":15,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"ldc.r8\",\"opCode\":3,\"operand\":4,\"ilOffset\":24,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":33,\"resultType\":\"VirtualDispatch/Rectangle\",\"callee\":\"VirtualDispatch/Rectangle::.ctor:System.Void(System.Double,System.Double)\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"method\",\"subjectId\":\"VirtualDispatch/Rectangle::.ctor:System.Void(System.Double,System.Double)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/Rectangle\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"VirtualDispatch/Shape\",\"implementedInterfaceSubjectIds\":[\"VirtualDispatch/IShape\"],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":38,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_VIRT_CIRCLE=\",\"ilOffset\":39,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":44,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":45,\"resultType\":\"System.String\",\"callee\":\"VirtualDispatch/DispatchSubjects::TestVirtualCall:System.String(Shape)\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"method\",\"subjectId\":\"VirtualDispatch/DispatchSubjects::TestVirtualCall:System.String(Shape)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/DispatchSubjects::TestVirtualCall:System.String(Shape)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"VirtualDispatch_DispatchSubjects_TestVirtualCall_Shape\",\"targetParameterCount\":1,\"targetReturnType\":\"System.String\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":50,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":55,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_VIRT_RECT=\",\"ilOffset\":60,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":65,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":66,\"resultType\":\"System.String\",\"callee\":\"VirtualDispatch/DispatchSubjects::TestVirtualCall:System.String(Shape)\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"method\",\"subjectId\":\"VirtualDispatch/DispatchSubjects::TestVirtualCall:System.String(Shape)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/DispatchSubjects::TestVirtualCall:System.String(Shape)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"VirtualDispatch_DispatchSubjects_TestVirtualCall_Shape\",\"targetParameterCount\":1,\"targetReturnType\":\"System.String\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":71,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":76,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":81,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":82,\"resultType\":\"System.Double\",\"callee\":\"VirtualDispatch/DispatchSubjects::TestInterfaceCall:System.Double(IShape)\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"method\",\"subjectId\":\"VirtualDispatch/DispatchSubjects::TestInterfaceCall:System.Double(IShape)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/DispatchSubjects::TestInterfaceCall:System.Double(IShape)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"VirtualDispatch_DispatchSubjects_TestInterfaceCall_IShape\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Double\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":87,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":88,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":89,\"resultType\":\"System.Double\",\"callee\":\"VirtualDispatch/DispatchSubjects::TestInterfaceCall:System.Double(IShape)\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"method\",\"subjectId\":\"VirtualDispatch/DispatchSubjects::TestInterfaceCall:System.Double(IShape)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/DispatchSubjects::TestInterfaceCall:System.Double(IShape)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"VirtualDispatch_DispatchSubjects_TestInterfaceCall_IShape\",\"targetParameterCount\":1,\"targetReturnType\":\"System.Double\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":94,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_IFACE_CIRCLE_AREA=\",\"ilOffset\":95,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":100,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":101,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":4,\"ilOffset\":102,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":4,\"ilOffset\":104,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":106,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":111,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":116,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_IFACE_RECT_AREA=\",\"ilOffset\":121,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":126,\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":127,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":4,\"ilOffset\":128,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":4,\"ilOffset\":130,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":132,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":137,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":142,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_IFACE_COLOR=\",\"ilOffset\":147,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":152,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":153,\"resultType\":\"System.String\",\"callee\":\"VirtualDispatch/DispatchSubjects::TestDerivedInterfaceCall:System.String(IColoredShape)\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"method\",\"subjectId\":\"VirtualDispatch/DispatchSubjects::TestDerivedInterfaceCall:System.String(IColoredShape)\"},\"targetRefer"
    "ence\":{\"kind\":3,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/DispatchSubjects::TestDerivedInterfaceCall:System.String(IColoredShape)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"VirtualDispatch_DispatchSubjects_TestDerivedInterfaceCall_IColoredShape\",\"targetParameterCount\":1,\"targetReturnType\":\"System.String\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":158,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":163,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_IFACE_GETNAME=\",\"ilOffset\":168,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":173,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":174,\"resultType\":\"System.String\",\"callee\":\"VirtualDispatch/IShape::GetName:System.String()\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"method\",\"subjectId\":\"VirtualDispatch/IShape::GetName:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/IShape::GetName:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"VirtualDispatch_IShape_GetName\",\"targetParameterCount\":0,\"targetReturnType\":\"System.String\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":179,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":184,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_DONE=ok\",\"ilOffset\":189,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":194,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":199,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"VirtualDispatch\"}"
    ;
static const char kMethodJson_11[] = "{\"methodId\":\"virtual-dispatch.rectangle.ctor\",\"subjectId\":\"VirtualDispatch/Rectangle::.ctor:System.Void(System.Double,System.Double)\",\"signature\":\"System.Void Rectangle::.ctor(System.Double,System.Double)\",\"identity\":{\"assemblyName\":\"VirtualDispatch\",\"declaringTypeSubjectId\":\"VirtualDispatch/Rectangle\",\"definitionSubjectId\":\"VirtualDispatch/Rectangle::.ctor:System.Void(System.Double,System.Double)\",\"subjectId\":\"VirtualDispatch/Rectangle::.ctor:System.Void(System.Double,System.Double)\",\"methodId\":\"virtual-dispatch.rectangle.ctor\",\"signature\":\"System.Void Rectangle::.ctor(System.Double,System.Double)\"},\"nativeSymbol\":\"VirtualDispatch_Rectangle__ctor_System_Double_System_Double\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":9,\"typeShape\":2},{\"carrierKindCode\":9,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"VirtualDispatch/Rectangle\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"VirtualDispatch/Shape::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"method\",\"subjectId\":\"VirtualDispatch/Shape::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/Shape::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"VirtualDispatch_Shape__ctor\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Void\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":6,\"resultType\":\"VirtualDispatch/Rectangle\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":7,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"stfld\",\"opCode\":11,\"operand\":\"VirtualDispatch/Rectangle::_w\",\"ilOffset\":8,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"field\",\"subjectId\":\"VirtualDispatch/Rectangle::_w\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/Rectangle::_w\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"VirtualDispatch/Rectangle\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Double\"},\"runtimeServiceKind\":3,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":13,\"resultType\":\"VirtualDispatch/Rectangle\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":2,\"ilOffset\":14,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"stfld\",\"opCode\":11,\"operand\":\"VirtualDispatch/Rectangle::_h\",\"ilOffset\":15,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"field\",\"subjectId\":\"VirtualDispatch/Rectangle::_h\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/Rectangle::_h\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"VirtualDispatch/Rectangle\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Double\"},\"runtimeServiceKind\":3,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":20,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"VirtualDispatch\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"virtual-dispatch.rectangle.area\",\"subjectId\":\"VirtualDispatch/Rectangle::Area:System.Double()\",\"signature\":\"System.Double Rectangle::Area()\",\"identity\":{\"assemblyName\":\"VirtualDispatch\",\"declaringTypeSubjectId\":\"VirtualDispatch/Rectangle\",\"definitionSubjectId\":\"VirtualDispatch/Rectangle::Area:System.Double()\",\"subjectId\":\"VirtualDispatch/Rectangle::Area:System.Double()\",\"methodId\":\"virtual-dispatch.rectangle.area\",\"signature\":\"System.Double Rectangle::Area()\"},\"nativeSymbol\":\"VirtualDispatch_Rectangle_Area\",\"isStatic\":false,\"returnType\":\"System.Double\",\"returnAbi\":{\"carrierKindCode\":9,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"VirtualDispatch/Rectangle\",\"isPreserveSig\":false},{\"op\":\"ldfld\",\"opCode\":10,\"operand\":\"VirtualDispatch/Rectangle::_w\",\"ilOffset\":1,\"resultType\":\"System.Double\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"field\",\"subjectId\":\"VirtualDispatch/Rectangle::_w\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/Rectangle::_w\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"VirtualDispatch/Rectangle\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Double\"},\"runtimeServiceKind\":2,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":6,\"resultType\":\"VirtualDispatch/Rectangle\",\"isPreserveSig\":false},{\"op\":\"ldfld\",\"opCode\":10,\"operand\":\"VirtualDispatch/Rectangle::_h\",\"ilOffset\":7,\"resultType\":\"System.Double\",\"reference\":{\"assemblyName\":\"VirtualDispatch\",\"subjectKind\":\"field\",\"subjectId\":\"VirtualDispatch/Rectangle::_h\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"VirtualDispatch\",\"subjectId\":\"VirtualDispatch/Rectangle::_h\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"VirtualDispatch/Rectangle\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Double\"},\"runtimeServiceKind\":2,\"isPreserveSig\":false},{\"op\":\"mul\",\"opCode\":27,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":13,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"VirtualDispatch\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"virtual-dispatch.rectangle.get-name\",\"subjectId\":\"VirtualDispatch/Rectangle::GetName:System.String()\",\"signature\":\"System.String Rectangle::GetName()\",\"identity\":{\"assemblyName\":\"VirtualDispatch\",\"declaringTypeSubjectId\":\"VirtualDispatch/Rectangle\",\"definitionSubjectId\":\"VirtualDispatch/Rectangle::GetName:System.String()\",\"subjectId\":\"VirtualDispatch/Rectangle::GetName:System.String()\",\"methodId\":\"virtual-dispatch.rectangle.get-name\",\"signature\":\"System.String Rectangle::GetName()\"},\"nativeSymbol\":\"VirtualDispatch_Rectangle_GetName\",\"isStatic\":false,\"returnType\":\"System.String\",\"returnAbi\":{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"Rectangle\",\"ilOffset\":0,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"VirtualDispatch\"}";
static const char kMethodJson_14[] = "{\"methodId\":\"virtual-dispatch.shape.ctor\",\"subjectId\":\"VirtualDispatch/Shape::.ctor:System.Void()\",\"signature\":\"System.Void Shape::.ctor()\",\"identity\":{\"assemblyName\":\"VirtualDispatch\",\"declaringTypeSubjectId\":\"VirtualDispatch/Shape\",\"definitionSubjectId\":\"VirtualDispatch/Shape::.ctor:System.Void()\",\"subjectId\":\"VirtualDispatch/Shape::.ctor:System.Void()\",\"methodId\":\"virtual-dispatch.shape.ctor\",\"signature\":\"System.Void Shape::.ctor()\"},\"nativeSymbol\":\"VirtualDispatch_Shape__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"VirtualDispatch/Shape\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"VirtualDispatch\"}";
static const char kMethodJson_15[] = "{\"methodId\":\"virtual-dispatch.shape.get-name\",\"subjectId\":\"VirtualDispatch/Shape::GetName:System.String()\",\"signature\":\"System.String Shape::GetName()\",\"identity\":{\"assemblyName\":\"VirtualDispatch\",\"declaringTypeSubjectId\":\"VirtualDispatch/Shape\",\"definitionSubjectId\":\"VirtualDispatch/Shape::GetName:System.String()\",\"subjectId\":\"VirtualDispatch/Shape::GetName:System.String()\",\"methodId\":\"virtual-dispatch.shape.get-name\",\"signature\":\"System.String Shape::GetName()\"},\"nativeSymbol\":\"VirtualDispatch_Shape_GetName\",\"isStatic\":false,\"returnType\":\"System.String\",\"returnAbi\":{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"Shape\",\"ilOffset\":0,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"VirtualDispatch\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[16] =
{
    { kMethodJson_0, 1863u, 0x0000000Fu, 0u },
    { kMethodJson_1, 1262u, 0x0000000Eu, 0u },
    { kMethodJson_2, 2682u, 0x00000015u, 0u },
    { kMethodJson_3, 2720u, 0x00000017u, 0u },
    { kMethodJson_4, 1216u, 0x00000018u, 0u },
    { kMethodJson_5, 1211u, 0x00000016u, 0u },
    { kMethodJson_6, 2292u, 0x0000001Eu, 0u },
    { kMethodJson_7, 2079u, 0x0000001Du, 0u },
    { kMethodJson_8, 2118u, 0x0000001Cu, 0u },
    { kMethodJson_9, 1729u, 0x0000000Du, 0u },
    { kMethodJson_10, 19489u, 0x0000000Cu, 0u },
    { kMethodJson_11, 3639u, 0x00000019u, 0u },
    { kMethodJson_12, 2546u, 0x0000001Bu, 0u },
    { kMethodJson_13, 1241u, 0x0000001Au, 0u },
    { kMethodJson_14, 1709u, 0x00000014u, 0u },
    { kMethodJson_15, 1201u, 0x00000013u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 16u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}