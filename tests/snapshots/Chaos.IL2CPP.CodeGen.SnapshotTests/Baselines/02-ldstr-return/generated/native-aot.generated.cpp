#include <chaos/common.h>
#include <chaos/type_info.h>
#include "runtime_core.h"
#include "codegen_bridge.h"
#include "module_registry.h"
#include "abi_manifest.h"
#include "hotpatch_table.h"
#include "runtime_vtable.h"
#include "runtime_instantiation.h"
#include "load_store_chaos_bridge.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

namespace chaos::il2cpp::codegen::SnapshotTestFixtures {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level



constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_managed_array = 1;
inline TypeInfo chaos_type_info_managed_array = { nullptr, nullptr, 1ULL, 0, 32, 2, 0 };

struct chaos_managed_array
{
	FatHeader header{};
	CHAOS_IL2CPP_UINT8 element_type_shape = 0;
	const TypeInfo* element_type_info = nullptr;
	CHAOS_IL2CPP_INTPTR length = 0;
	CHAOS_IL2CPP_INTPTR* elements = nullptr;
};

constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_reference = 1;
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_value = 2;
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_interface = 3;

constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = 1;

CHAOS_IL2CPP_INTPTR chaos_normalize_native_int_argument(CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
	if ((chaos_value & chaos_managed_pointer_local_slot_tag) == 0)
	{
		return chaos_value;
	}

	auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_value & ~chaos_managed_pointer_local_slot_tag));
	return *chaos_slot;
}

template <typename TValue>
TValue* chaos_resolve_managed_value_pointer(CHAOS_IL2CPP_INTPTR chaos_managed_pointer)
{
	if ((chaos_managed_pointer & chaos_managed_pointer_local_slot_tag) != 0)
	{
		auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_managed_pointer & ~chaos_managed_pointer_local_slot_tag));
		if (*chaos_slot == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			*chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new TValue{});
		}
		return reinterpret_cast<TValue*>(*chaos_slot);
	}

	return reinterpret_cast<TValue*>(chaos_managed_pointer);
}

inline TypeInfoHot chaos_type_info_System_Private_CoreLib_System_Reflection_Assembly = { nullptr, nullptr, 5474029880995115448ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1 };
inline TypeInfoWarm chaos_type_info_warm_System_Private_CoreLib_System_Reflection_Assembly = { nullptr, nullptr, 0, 0, 0, 0 };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);inline TypeInfoHot chaos_type_info_System_Private_CoreLib_System_Reflection_AssemblyName = { nullptr, nullptr, 17082367815459723707ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1 };
inline TypeInfoWarm chaos_type_info_warm_System_Private_CoreLib_System_Reflection_AssemblyName = { nullptr, nullptr, 0, 0, 0, 0 };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);inline TypeInfoHot chaos_type_info_System_Private_CoreLib_System_Reflection_ConstructorInfo = { nullptr, nullptr, 4137207361503509124ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1 };
inline TypeInfoWarm chaos_type_info_warm_System_Private_CoreLib_System_Reflection_ConstructorInfo = { nullptr, nullptr, 0, 0, 0, 0 };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);inline TypeInfoHot chaos_type_info_System_Private_CoreLib_System_Reflection_FieldInfo = { nullptr, nullptr, 17040031516751226236ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1 };
inline TypeInfoWarm chaos_type_info_warm_System_Private_CoreLib_System_Reflection_FieldInfo = { nullptr, nullptr, 0, 0, 0, 0 };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);inline TypeInfoHot chaos_type_info_System_Private_CoreLib_System_Reflection_MethodInfo = { nullptr, nullptr, 10748947813473285525ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 1 };
inline TypeInfoWarm chaos_type_info_warm_System_Private_CoreLib_System_Reflection_MethodInfo = { nullptr, nullptr, 0, 0, 0, 0 };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);inline TypeInfoHot chaos_type_info_System_Private_CoreLib_System_String = { nullptr, nullptr, 1782325859292956794ULL, 0u, 32 /* warm_delta */, 1 /* reference */, 2 };
inline TypeInfoWarm chaos_type_info_warm_System_Private_CoreLib_System_String = { nullptr, nullptr, 0, 0, 0, 0 };
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);
// ── Virtual method table arrays ──
inline void* chaos_vtable_resolve(const void** vtable, CHAOS_IL2CPP_UINT32 slot) noexcept
{
	if (vtable == nullptr) CHAOS_IL2CPP_ABORT();
	if (vtable[slot] == nullptr) CHAOS_IL2CPP_ABORT();
	return const_cast<void*>(vtable[slot]);
}

inline const TypeInfo* chaos_get_parent_type_info(const TypeInfo* chaos_ti) noexcept
{
	if (chaos_ti == nullptr) return nullptr;
	return chaos_ti->parent;
}

bool chaos_is_type_compatible(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_type_info) noexcept
{
	auto* chaos_current = chaos_actual_type_info;
	while (chaos_current != nullptr)
	{
		if (chaos_current == chaos_target_type_info || chaos_current->stable_id == chaos_target_type_info->stable_id)
		{
			return true;
		}

		chaos_current = chaos_current->parent;
	}

	return false;
}

bool chaos_type_implements_interface(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_interface_type_info) noexcept
{
	if (chaos_actual_type_info == nullptr || chaos_target_interface_type_info == nullptr)
	{
		return false;
	}

	const auto* chaos_warm = GetWarmPtr(chaos_actual_type_info);
	if (chaos_warm == nullptr) return false;

	if (chaos_warm->iface_count == 0 &&
		chaos_warm->runtime_iface_count == 0)
	{
		return false;
	}

	for (CHAOS_IL2CPP_UINT32 chaos_i = 0; chaos_i < chaos_warm->iface_count; chaos_i++)
	{
		if (chaos_warm->iface_map[chaos_i].iface_stable_id == chaos_target_interface_type_info->stable_id)
		{
			return true;
		}
	}

	for (CHAOS_IL2CPP_UINT32 chaos_i = 0; chaos_i < chaos_warm->runtime_iface_count; chaos_i++)
	{
		if (chaos_warm->runtime_iface_map[chaos_i].iface_stable_id == chaos_target_interface_type_info->stable_id)
		{
			return true;
		}
	}

	return false;
}

bool chaos_does_type_implement_interface(const TypeInfo* chaos_actual_type_info, const TypeInfo* chaos_target_interface_type_info) noexcept
{
	auto* chaos_current = chaos_actual_type_info;
	while (chaos_current != nullptr)
	{
		if (chaos_type_implements_interface(chaos_current, chaos_target_interface_type_info))
		{
			return true;
		}

		chaos_current = chaos_current->parent;
	}

	return false;
}

bool chaos_is_array_type_compatible(
	CHAOS_IL2CPP_UINT8 chaos_actual_element_shape,
	const TypeInfo* chaos_actual_element_type_info,
	CHAOS_IL2CPP_UINT8 chaos_target_element_shape,
	const TypeInfo* chaos_target_element_type_info) noexcept
{
	if (chaos_actual_element_shape == chaos_type_shape_reference)
	{
		if (chaos_target_element_shape == chaos_type_shape_reference)
		{
			return chaos_is_type_compatible(chaos_actual_element_type_info, chaos_target_element_type_info);
		}

		if (chaos_target_element_shape == chaos_type_shape_interface)
		{
			return chaos_does_type_implement_interface(chaos_actual_element_type_info, chaos_target_element_type_info);
		}

		return false;
	}

	return chaos_actual_element_shape == chaos_target_element_shape
		&& chaos_actual_element_type_info == chaos_target_element_type_info;
}

bool chaos_is_array_store_compatible(const chaos_managed_array* chaos_array, CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
	if (chaos_array == nullptr)
	{
		return false;
	}

	if (chaos_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		return true;
	}

	// StringId fast path: materialized value carries a heap pointer;
	// this line is reached only if caller skipped materialization.
	if (chaos_is_string_id(chaos_value))
	{
		return chaos_array->element_type_shape == chaos_type_shape_reference            && chaos_is_type_compatible(&chaos_type_info_System_Private_CoreLib_System_String, chaos_array->element_type_info);
	}

	auto* chaos_header = reinterpret_cast<FatHeader*>(chaos_value);
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
	FatHeader header{};
	CHAOS_IL2CPP_INTPTR runtime_assembly_name_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName
{
	FatHeader header{};
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo
{
	FatHeader header{};
	CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_method_handle = 0;
	CHAOS_IL2CPP_INTPTR generic_definition_method_handle = 0;
	CHAOS_IL2CPP_INTPTR generic_argument_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo
{
	FatHeader header{};
	CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo
{
	FatHeader header{};
	CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_method_handle = 0;
	CHAOS_IL2CPP_INTPTR generic_definition_method_handle = 0;
	CHAOS_IL2CPP_INTPTR generic_argument_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_String
{
	FatHeader header{};
	CHAOS_IL2CPP_INTPTR length = 0;
	const char* utf8_data = nullptr;
	CHAOS_IL2CPP_UINT64 string_id = 0u;  // stable StringId, 0 = not yet materialized
};

const char* chaos_reflection_get_string_utf8(CHAOS_IL2CPP_INTPTR chaos_string_value) noexcept
{
	if (chaos_string_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
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


CHAOS_IL2CPP_INTPTR chaos_string_materialize(CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
	if (chaos_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
	{
		return static_cast<CHAOS_IL2CPP_INTPTR>(0);
	}

	if (!chaos_is_string_id(chaos_value))
	{
		return chaos_value;
	}

	const auto chaos_id = chaos_extract_string_id(chaos_value);
	const auto chaos_view = chaos::il2cpp::string_table::Resolve(chaos_id);

	auto* chaos_string = static_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos::il2cpp::runtime_core::GcAllocate(sizeof(
chaos_type_System_Private_CoreLib_System_String)));
	chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
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
		{ 7993990320990026837U, "Hello, World!", 13u },
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

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };
static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };
static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };
static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void SnapshotTestFixtures_Helper_ConsumeString(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StringOps_UseString(void);

// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.
// NOTE: reinterpret_cast is needed because MSVC rejects &anon_struct.header
// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[3] = {
	0u,
	1u,
	1u
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[2];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		2u,
		1u,
		4274733347u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 1u },  // SnapshotTestFixtures_Helper_ConsumeString
		{ 1u, 0u },  // SnapshotTestFixtures_StringOps_UseString
	},
	{
		2u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest = reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);

// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[66] = {
		0u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1536u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		12u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1728u,
		1548u,
		1536u,
		1536u,
		1548u,
		1556u,
		1536u,
		1548u,
		1545u,
		1548u,
		1548u,
		1548u,
		1548u,
		1536u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1536u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
	};

	static constexpr const char* s_type_names[66] = {
		"<Module>",
		"ArithmeticOps",
		"StringOps",
		"VoidCaller",
		"MathHelper",
		"FlowControl",
		"InstanceHelper",
		"CallVirtHelper",
		"BoxingHelper",
		"FieldHelper",
		"LoopHelper",
		"ConversionHelper",
		"Helper",
		"SwitchHelper",
		"LdftnHelper",
		"TargetHelper",
		"ArrayHelper",
		"EhHelper",
		"GenericHelper",
		"Wrapper`1",
		"DelegateHelper",
		"BaseClass",
		"DerivedClass",
		"VirtualHelper",
		"IMarker",
		"MarkedClass",
		"TypeCheckHelper",
		"SmallStruct",
		"SizeOfHelper",
		"IndirectHelper",
		"ArithmeticCompareHelper",
		"BranchDupHelper",
		"HasInstanceFields",
		"InstanceFieldHelper",
		"StaticFieldWriteHelper",
		"VirtualDispatchHelper",
		"BitwiseHelper",
		"ShiftHelper",
		"ArrayLengthHelper",
		"ValueTypeHelper",
		"LocalAllocHelper",
		"MulDivHelper",
		"BranchCompareHelper",
		"FloatOpsHelper",
		"ConvWideHelper",
		"NegShiftHelper",
		"LdcI8Helper",
		"ConvSmallIntHelper",
		"LdindI1Helper",
		"BranchCompareBHelper",
		"HasFields",
		"AddressHelper",
		"LdtokenHelper",
		"LdelemaHelper",
		"LdargaHelper",
		"CpobjHelper",
		"StindNarrowHelper",
		"CallvirtHelper",
		"UnboxHelper",
		"CpblkHelper",
		"LdindUnsignedHelper",
		"LdindI8Helper",
		"LdindWideHelper",
		"StindWideHelper",
		"ArrayRefHelper",
		"<>O",
	};

	static constexpr const char* s_type_namespaces[66] = {
		"",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[66] = {
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
		0u,
		0u,
		0u,
		33554454u,
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
		0u,
		0u,
		0u,
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[66] = {
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

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[1] = {
		33554498u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[67] = {
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
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[67] = {
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
		/* .name_utf8         = */ "SnapshotTestFixtures",
		/* .image             = */ nullptr,  // Tier 2 metadata — deferred
		/* .type_flags        = */ s_type_flags,
		/* .type_names        = */ s_type_names,
		/* .type_namespaces   = */ s_type_namespaces,
		/* .type_parent_tokens= */ s_type_parent_tokens,
		/* .type_info_ptrs    = */ s_type_info_ptrs,
		/* .nested_type_children= */ s_nested_type_children,
		/* .nested_type_offset = */ s_nested_type_offset,
		/* .generic_param_constraint_data= */ nullptr,
		/* .generic_param_constraint_offset= */ s_generic_param_constraint_offset,
		/* .type_count        = */ 66u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("SnapshotTestFixtures", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[2] = {
	{ "ConsumeString", 0x00000004u, 1u },  // Helper
	{ "UseString", 0x00000002u, 0u },  // StringOps
};

// Type name index entries
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "Helper", 0u, 1u },
	{ "StringOps", 1u, 1u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[2] = {
	{ 0x00000002u, 1u },
	{ 0x00000004u, 0u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[2] = {
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_Helper_ConsumeString), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // Helper::ConsumeString
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_StringOps_UseString), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0u },  // StringOps::UseString
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"SnapshotTestFixtures",
	s_hotpatch_types,
	2u,
	s_hotpatch_methods,
	2u,
	s_hotpatch_slots,
	2u,
	s_hotpatch_entries,
	2u,
};

// Register hotpatch table with the runtime on load
static const CHAOS_IL2CPP_UINT32 s_hotpatch_registered = []()
{
	::chaos::il2cpp::runtime_core::RegisterHotpatchModule(
		&s_hotpatch_module);
	return 1u;
}();

// (no method AOT entries for this module)

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

// Managed method: SnapshotTestFixtures/Helper::ConsumeString(System.String)
extern "C" void SnapshotTestFixtures_Helper_ConsumeString(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: SnapshotTestFixtures/StringOps::UseString()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_StringOps_UseString(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
		chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
		chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(13);
		chaos_string->utf8_data = "Hello, World!";
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		auto& _d0 = s_hotpatch_entries[0];
		if (_d0.flags & kHotpatchActive)
		{
			alignas(16) uint8_t _d_ab[8];
			ArgBuffer _d_bw(_d_ab);
			_d_bw._d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
			::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				_d0.method_key, _d_ab, nullptr);
		}
		else
		{
			SnapshotTestFixtures_Helper_ConsumeString(chaos_arg_0);
		}
	}
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}


// Native AOT entry for SnapshotTestFixtures/StringOps::UseString:System.Int32()
// Managed symbol: SnapshotTestFixtures_StringOps_UseString
extern "C" CHAOS_IL2CPP_INT32 RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return SnapshotTestFixtures_StringOps_UseString();
}

}  // namespace chaos::il2cpp::codegen::SnapshotTestFixtures
#pragma warning(pop)