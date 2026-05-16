#include <chaos/common.h>
#include <chaos/type_info.h>
#include "runtime_core.h"
#include "codegen_bridge.h"
#include "module_registry.h"
#include "abi_manifest.h"
#include "hotpatch_table.h"
#include "runtime_vtable.h"
#include "runtime_instantiation.h"
#include "reflection_query_model.h"
#include "load_store_chaos_bridge.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

namespace chaos::il2cpp::codegen::ThreadingMonitorInterlockedSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level



constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_managed_array = 1;
inline TypeInfoV0 chaos_type_info_managed_array = {{ nullptr, nullptr, 1ULL, 0, 32, 2, 0 }, { nullptr, nullptr, 0, 0, 0, 0 }};

struct chaos_managed_array
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_UINT8 element_type_shape = 0;
	const TypeInfo* element_type_info = nullptr;
	CHAOS_IL2CPP_INTPTR length = 0;
	CHAOS_IL2CPP_INTPTR* elements = nullptr;
};

constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_reference = 1;
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_value = 2;
constexpr CHAOS_IL2CPP_UINT8 chaos_type_shape_interface = 3;

#ifdef CHAOS_IL2CPP_VERIFY_MODE
constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = 0;
#else
constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = 1;
#endif

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
			*chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NEW_GC(TValue));
		}
		return reinterpret_cast<TValue*>(*chaos_slot);
	}

	return reinterpret_cast<TValue*>(chaos_managed_pointer);
}

inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {{nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {{nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);inline TypeInfoV0 chaos_mt_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects = {{nullptr, nullptr, 4882207410565625576ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(4882207410565625576ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Int32 = {{nullptr, nullptr, 11009693519287992193ULL, 0u, 32, 2, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_boxed_type_id_System_Private_CoreLib_System_Int32 = static_cast<CHAOS_IL2CPP_INTPTR>(11009693519287992193ULL);

// ── Virtual method table arrays ──
inline void* chaos_vtable_resolve(const void** vtable, CHAOS_IL2CPP_UINT32 slot) noexcept
{
	if (vtable == nullptr) CHAOS_IL2CPP_FAIL();
	if (vtable[slot] == nullptr) CHAOS_IL2CPP_FAIL();
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
		return chaos_array->element_type_shape == chaos_type_shape_reference            && chaos_is_type_compatible(&chaos_mt_System_Private_CoreLib_System_String.hot, chaos_array->element_type_info);
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

struct chaos_type_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects
{
	ThinLockableHeader header{};
};

struct chaos_boxed_type_System_Private_CoreLib_System_Int32
{
	PureTypeHeader header{};
	CHAOS_IL2CPP_INTPTR value = 0;
};

CHAOS_IL2CPP_INT32 chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = 0;

static void chaos_external_runtime_System_Private_CoreLib_Interlocked__MemoryBarrier_System_Void__(void)
{
	ChaosInterlockedMemoryBarrier();
}

static void chaos_external_runtime_System_Private_CoreLib_Monitor__Exit_System_Void_System_Object_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	chaos_monitor_exit(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_Monitor__TryEnter_System_Boolean_System_Object_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	return ChaosMonitorTryEnter(chaos_fn_arg_0, chaos_fn_arg_1);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_Volatile__Read_System_Int32_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosVolatileRead(chaos_fn_arg_0);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_0(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_1(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_4(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_5(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_6(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_7(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_8(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_9(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_10(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_11(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Run(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const CHAOS_IL2CPP_INT32 kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[16] = {

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

};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[15];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		15u,
		1u,
		1705251057u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{

		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_0

		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_1

		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2

		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3

		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_4

		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_5

		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_6

		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_7

		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_8

		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_9

		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_10

		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_11

		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12

		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13

		{ 0u, 1u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Run

	},
	{


		1u,


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
		"ThreadingMonitorInterlockedSubjects",
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
		&chaos_mt_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects.hot,
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
		/* .name_utf8         = */ "ThreadingMonitorInterlockedSubjects",
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
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("ThreadingMonitorInterlockedSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────

// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[15] = {


	{ "CustomEntrySubject_0", 0x00000008u, 0u },  // ThreadingMonitorInterlockedSubjects

	{ "CustomEntrySubject_1", 0x00000009u, 0u },  // ThreadingMonitorInterlockedSubjects

	{ "Subject_2", 0x00000004u, 0u },  // ThreadingMonitorInterlockedSubjects

	{ "Subject_3", 0x00000005u, 0u },  // ThreadingMonitorInterlockedSubjects

	{ "CustomEntrySubject_4", 0x0000000Au, 0u },  // ThreadingMonitorInterlockedSubjects

	{ "CustomEntrySubject_5", 0x0000000Bu, 0u },  // ThreadingMonitorInterlockedSubjects

	{ "CustomEntrySubject_6", 0x0000000Cu, 0u },  // ThreadingMonitorInterlockedSubjects

	{ "CustomEntrySubject_7", 0x0000000Du, 0u },  // ThreadingMonitorInterlockedSubjects

	{ "CustomEntrySubject_8", 0x0000000Eu, 0u },  // ThreadingMonitorInterlockedSubjects

	{ "CustomEntrySubject_9", 0x0000000Fu, 0u },  // ThreadingMonitorInterlockedSubjects

	{ "CustomEntrySubject_10", 0x00000010u, 0u },  // ThreadingMonitorInterlockedSubjects

	{ "CustomEntrySubject_11", 0x00000011u, 0u },  // ThreadingMonitorInterlockedSubjects

	{ "Subject_12", 0x00000006u, 0u },  // ThreadingMonitorInterlockedSubjects

	{ "Subject_13", 0x00000007u, 0u },  // ThreadingMonitorInterlockedSubjects

	{ "Run", 0x00000003u, 1u },  // ThreadingMonitorInterlockedSubjects


};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {

	{ "ThreadingMonitorInterlockedSubjects", "", 0u, 15u },

};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[15] = {

	{ 0x00000003u, 14u },

	{ 0x00000004u, 2u },

	{ 0x00000005u, 3u },

	{ 0x00000006u, 12u },

	{ 0x00000007u, 13u },

	{ 0x00000008u, 0u },

	{ 0x00000009u, 1u },

	{ 0x0000000Au, 4u },

	{ 0x0000000Bu, 5u },

	{ 0x0000000Cu, 6u },

	{ 0x0000000Du, 7u },

	{ 0x0000000Eu, 8u },

	{ 0x0000000Fu, 9u },

	{ 0x00000010u, 10u },

	{ 0x00000011u, 11u },

};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[15] = {

	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_0

	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_1

	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::Subject_2

	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::Subject_3

	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_4

	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_5

	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_6

	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_7

	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_8

	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_9

	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_10

	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_11

	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::Subject_12

	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::Subject_13

	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Run), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingMonitorInterlockedSubjects::Run

};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ThreadingMonitorInterlockedSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	15u,
	s_hotpatch_slots,
	15u,
	s_hotpatch_entries,
	15u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[15] = {

	"System.Private.CoreLib/System.Object::.ctor:System.Void()",

	"System.Threading/Interlocked::Add:System.Int32(System.Int32&,System.Int32)",

	"System.Threading/Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)",

	"System.Threading/Interlocked::Decrement:System.Int32(System.Int32&)",

	"System.Threading/Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)",

	"System.Threading/Interlocked::Increment:System.Int32(System.Int32&)",

	"System.Threading/Interlocked::MemoryBarrier:System.Void()",

	"System.Threading/Monitor::Enter:System.Void(System.Object)",

	"System.Threading/Monitor::Exit:System.Void(System.Object)",

	"System.Threading/Monitor::Pulse:System.Void(System.Object)",

	"System.Threading/Monitor::PulseAll:System.Void(System.Object)",

	"System.Threading/Monitor::TryEnter:System.Boolean(System.Object)",

	"System.Threading/Monitor::TryEnter:System.Boolean(System.Object,System.Int32)",

	"System.Threading/Monitor::Wait:System.Boolean(System.Object,System.Int32)",

	"System.Threading/Volatile::Read:System.Int32(System.Int32&)",

};

extern "C" void* kChaosExternalRuntimeFnTable[15] = {


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


};

extern "C" CHAOS_IL2CPP_INT32 kChaosExternalRuntimeCount = 15;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[15])() = {

	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_0),

	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_1),

	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2),

	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3),

	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_4),

	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_5),

	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_6),

	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_7),

	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_8),

	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_9),

	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_10),

	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_11),

	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12),

	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13),

	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Run),

};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
static void (*kBenchmarkWrappers[15])() = {

	[]() { kAotMethods[0](); },

	[]() { kAotMethods[1](); },

	[]() { kAotMethods[2](); },

	[]() { kAotMethods[3](); },

	[]() { kAotMethods[4](); },

	[]() { kAotMethods[5](); },

	[]() { kAotMethods[6](); },

	[]() { kAotMethods[7](); },

	[]() { kAotMethods[8](); },

	[]() { kAotMethods[9](); },

	[]() { kAotMethods[10](); },

	[]() { kAotMethods[11](); },

	[]() { kAotMethods[12](); },

	[]() { kAotMethods[13](); },

	[]() { reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[14])(static_cast<CHAOS_IL2CPP_INTPTR>(0)); },

};

// Single-method dispatch via hotpatch dispatch table.
extern "C" CHAOS_IL2CPP_INT32 RunNativeAot(
	CHAOS_IL2CPP_INT32 chaos_entry_index)
{
	if (chaos_entry_index < 0 || chaos_entry_index >= kAotMethodCount)
		return -1;
	auto& entry = s_hotpatch_entries[chaos_entry_index];
	if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry)
		&& !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry))
	{
		CHAOS_IL2CPP_UINT64 __chaos_args[4] = {}; CHAOS_IL2CPP_UINT64 __chaos_ret[2] = {};
		chaos::il2cpp::runtime_core::InterpreterEntryDirect(
			entry.method_key, __chaos_args, __chaos_ret);
	} else {
		reinterpret_cast<void(*)()>(entry.direct_ptr)();
	}
	return 0;
}

// All-methods loop: run every method and return a bitmask of failures.
extern "C" CHAOS_IL2CPP_INT32 RunNativeAotAll()
{
	CHAOS_IL2CPP_INT32 result = 0;
	for (CHAOS_IL2CPP_INT32 i = 0; i < kAotMethodCount; i++) {
		auto& entry = s_hotpatch_entries[i];
		if (chaos::il2cpp::runtime_core::HotpatchIsActive(entry)
			&& !chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(entry))
		{
			CHAOS_IL2CPP_UINT64 __chaos_args[4] = {}; CHAOS_IL2CPP_UINT64 __chaos_ret[2] = {};
			chaos::il2cpp::runtime_core::InterpreterEntryDirect(
				entry.method_key, __chaos_args, __chaos_ret);
		} else {
			reinterpret_cast<void(*)()>(entry.direct_ptr)();
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
		reinterpret_cast<void(*)()>(entry.direct_ptr)();
	}
	return 0;
}

// Pure AOT benchmark: calls kAotMethods[i] directly, no hotpatch overhead.
extern "C" double BenchmarkMethod(
	CHAOS_IL2CPP_INT32 chaos_entry_index, CHAOS_IL2CPP_INT32 iterations) {
	if (chaos_entry_index < 0 || chaos_entry_index >= kAotMethodCount)
		return -1.0;
	auto start = CHAOS_IL2CPP_CHRONO_STEADY_CLOCK::now();
	for (CHAOS_IL2CPP_INT32 i = 0; i < iterations; i++) {
		kBenchmarkWrappers[chaos_entry_index]();
	}
	auto end = CHAOS_IL2CPP_CHRONO_STEADY_CLOCK::now();
	return CHAOS_IL2CPP_CHRONO_DURATION_CAST(
		CHAOS_IL2CPP_CHRONO_DURATION(double, CHAOS_IL2CPP_CHRONO_MILLI),
		end - start).count();
}
// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[15] = {

	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_0),

	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_1),

	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2),

	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3),

	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_4),

	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_5),

	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_6),

	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_7),

	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_8),

	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_9),

	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_10),

	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_11),

	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12),

	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13),

	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Run),

};


// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 15u,

	.reverse_pinvoke_wrappers  = nullptr,
	.reverse_pinvoke_wrapper_count = 0u,

	.invoker_pointers          = nullptr,
	.invoker_pointer_count     = 0u,
	.unresolved_virtual_calls = nullptr,
	.unresolved_virtual_call_count = 0u,
	.type_capabilities       = nullptr,
	.type_capability_count   = 0u,
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
	.image_name_utf8    = "ThreadingMonitorInterlockedSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.


static constexpr ReflectionQueryMethodDescriptor kReflMethods_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects[15] = {

	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u, nullptr },

	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u, nullptr },

	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u, nullptr },

	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u, nullptr },

	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u, nullptr },

	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_5:System.Void()", "CustomEntrySubject_5", "System.Void", 0, nullptr, 0u, nullptr },

	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u, nullptr },

	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u, nullptr },

	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u, nullptr },

	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_9:System.Void()", "CustomEntrySubject_9", "System.Void", 0, nullptr, 0u, nullptr },

	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_10:System.Void()", "CustomEntrySubject_10", "System.Void", 0, nullptr, 0u, nullptr },

	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_11:System.Void()", "CustomEntrySubject_11", "System.Void", 0, nullptr, 0u, nullptr },

	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u, nullptr },

	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u, nullptr },

	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Run:System.Void(System.Int32)", "Run", "System.Void", 0, nullptr, 0u, nullptr },

};


static constexpr ReflectionQueryTypeDescriptor kReflTypes[1] = {

	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects", "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects", "", "ThreadingMonitorInterlockedSubjects", "ThreadingMonitorInterlockedSubjects", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects, 15u, 0u },

};

static constexpr const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {

	&kReflTypes[0],

};

static constexpr ReflectionQueryImageDescriptor kReflImage = { "ThreadingMonitorInterlockedSubjects", kReflTypePtrs, 1u };

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
// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_0()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Object, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Object.hot;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		chaos_external_runtime_System_Private_CoreLib_Monitor__Exit_System_Void_System_Object_(chaos_arg_0);
	}
	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_1()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Object, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Object.hot;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		chaos_external_runtime_System_Private_CoreLib_Monitor__Exit_System_Void_System_Object_(chaos_arg_0);
	}
	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_2()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_boxed_type_System_Private_CoreLib_System_Int32 chaos_box_storage_0{};
	{
		const auto chaos_value = _s0;
		chaos_box_storage_0.header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
		chaos_box_storage_0.value = chaos_value;
	}
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_box_storage_0);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
		chaos_boxed_type_System_Private_CoreLib_System_Int32 chaos_box_storage_1{};
		{
			const auto chaos_value = _s1;
			chaos_box_storage_1.header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
			chaos_box_storage_1.value = chaos_value;
		}
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_box_storage_1);
		{
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[11])(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_3()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_boxed_type_System_Private_CoreLib_System_Int32 chaos_box_storage_0{};
	{
		const auto chaos_value = _s0;
		chaos_box_storage_0.header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
		chaos_box_storage_0.value = chaos_value;
	}
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_box_storage_0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_Monitor__TryEnter_System_Boolean_System_Object_System_Int32_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
		chaos_boxed_type_System_Private_CoreLib_System_Int32 chaos_box_storage_1{};
		{
			const auto chaos_value = _s1;
			chaos_box_storage_1.header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
			chaos_box_storage_1.value = chaos_value;
		}
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_box_storage_1);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
		{
			const auto chaos_raw_arg_1 = _s2;
			const auto chaos_arg_1 = chaos_raw_arg_1;
			const auto chaos_raw_arg_0 = _s1;
			const auto chaos_arg_0 = chaos_raw_arg_0;
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_Monitor__TryEnter_System_Boolean_System_Object_System_Int32_(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (_s1 != static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			}
			else
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			}
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
				const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
			}
			chaos_locals[0] = _s0;
			_s0 = chaos_locals[0];
			{
				if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
				{
				}
				else
				{
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					{
						auto chaos_value = _s2;
						chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
					}
				}
				return;
			}
		}
	}
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_4()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Object, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Object.hot;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		chaos_external_runtime_System_Private_CoreLib_Monitor__Exit_System_Void_System_Object_(chaos_arg_0);
	}
	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_5()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Object, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Object.hot;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[10])(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		chaos_external_runtime_System_Private_CoreLib_Monitor__Exit_System_Void_System_Object_(chaos_arg_0);
	}
	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_6()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_6(void)
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


	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Object, {});
		chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Object.hot;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0);
	}
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[13])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s0 = chaos_locals[0];
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		chaos_external_runtime_System_Private_CoreLib_Monitor__Exit_System_Void_System_Object_(chaos_arg_0);
	}
	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_7()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};
	CHAOS_IL2CPP_INTPTR _s7{};
	CHAOS_IL2CPP_INTPTR _s8{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[5])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_8()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};
	CHAOS_IL2CPP_INTPTR _s6{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_UINTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_UINTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left > chaos_right ? 1 : 0);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_9()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(99);
	{
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[4])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
			}
		}
		_s0 = chaos_locals[0];
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(99);
		{
			const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
			const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		{
			const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
			const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
		}
		chaos_locals[2] = _s0;
		_s0 = chaos_locals[2];
		{
			if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
			}
			else
			{
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s0;
					chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
				}
			}
			return;
		}
	}
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_10()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(99);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_raw_arg_2;
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[2])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
			}
		}
		_s0 = chaos_locals[0];
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(99);
		{
			const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
			const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		{
			const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
			const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
		}
		chaos_locals[2] = _s0;
		_s0 = chaos_locals[2];
		{
			if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
			{
			}
			else
			{
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s0;
					chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
				}
			}
			return;
		}
	}
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_11()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(10);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	{
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[1])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(15);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_12()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	{
		chaos_external_runtime_System_Private_CoreLib_Interlocked__MemoryBarrier_System_Void__();
	}
	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_13()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_Volatile__Read_System_Int32_System_Int32__(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_Volatile__Read_System_Int32_System_Int32__(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_right = static_cast<CHAOS_IL2CPP_INTPTR>(_s1);
		const auto chaos_left = static_cast<CHAOS_IL2CPP_INTPTR>(_s0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left == chaos_right ? 1 : 0);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	{
		if (_s0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))
		{
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Run(System.Int32)
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Run(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(_s0);
		switch (chaos_switch_value)
		{
			case 0:
			{
				{
					auto& _d0 = s_hotpatch_entries[0];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d0)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d0))
					{
						alignas(16) CHAOS_IL2CPP_UINT8 _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d0.method_key, _d_ab, nullptr);
					}
					else
					{
						ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_0();
					}
				}
				break;
			}
			case 1:
			{
				{
					auto& _d1 = s_hotpatch_entries[1];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d1)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d1))
					{
						alignas(16) CHAOS_IL2CPP_UINT8 _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d1.method_key, _d_ab, nullptr);
					}
					else
					{
						ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_1();
					}
				}
				break;
			}
			case 2:
			{
				{
					auto& _d2 = s_hotpatch_entries[2];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d2)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d2))
					{
						alignas(16) CHAOS_IL2CPP_UINT8 _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d2.method_key, _d_ab, nullptr);
					}
					else
					{
						ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2();
					}
				}
				break;
			}
			case 3:
			{
				{
					auto& _d3 = s_hotpatch_entries[3];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d3)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d3))
					{
						alignas(16) CHAOS_IL2CPP_UINT8 _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d3.method_key, _d_ab, nullptr);
					}
					else
					{
						ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3();
					}
				}
				break;
			}
			case 4:
			{
				{
					auto& _d4 = s_hotpatch_entries[4];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d4)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d4))
					{
						alignas(16) CHAOS_IL2CPP_UINT8 _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d4.method_key, _d_ab, nullptr);
					}
					else
					{
						ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_4();
					}
				}
				break;
			}
			case 5:
			{
				{
					auto& _d5 = s_hotpatch_entries[5];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d5)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d5))
					{
						alignas(16) CHAOS_IL2CPP_UINT8 _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d5.method_key, _d_ab, nullptr);
					}
					else
					{
						ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_5();
					}
				}
				break;
			}
			case 6:
			{
				{
					auto& _d6 = s_hotpatch_entries[6];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d6)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d6))
					{
						alignas(16) CHAOS_IL2CPP_UINT8 _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d6.method_key, _d_ab, nullptr);
					}
					else
					{
						ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_6();
					}
				}
				break;
			}
			case 7:
			{
				{
					auto& _d7 = s_hotpatch_entries[7];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d7)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d7))
					{
						alignas(16) CHAOS_IL2CPP_UINT8 _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d7.method_key, _d_ab, nullptr);
					}
					else
					{
						ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_7();
					}
				}
				break;
			}
			case 8:
			{
				{
					auto& _d8 = s_hotpatch_entries[8];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d8)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d8))
					{
						alignas(16) CHAOS_IL2CPP_UINT8 _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d8.method_key, _d_ab, nullptr);
					}
					else
					{
						ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_8();
					}
				}
				break;
			}
			case 9:
			{
				{
					auto& _d9 = s_hotpatch_entries[9];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d9)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d9))
					{
						alignas(16) CHAOS_IL2CPP_UINT8 _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d9.method_key, _d_ab, nullptr);
					}
					else
					{
						ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_9();
					}
				}
				break;
			}
			case 10:
			{
				{
					auto& _d10 = s_hotpatch_entries[10];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d10)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d10))
					{
						alignas(16) CHAOS_IL2CPP_UINT8 _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d10.method_key, _d_ab, nullptr);
					}
					else
					{
						ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_10();
					}
				}
				break;
			}
			case 11:
			{
				{
					auto& _d11 = s_hotpatch_entries[11];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d11)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d11))
					{
						alignas(16) CHAOS_IL2CPP_UINT8 _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d11.method_key, _d_ab, nullptr);
					}
					else
					{
						ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_11();
					}
				}
				break;
			}
			case 12:
			{
				{
					auto& _d12 = s_hotpatch_entries[12];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d12)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d12))
					{
						alignas(16) CHAOS_IL2CPP_UINT8 _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d12.method_key, _d_ab, nullptr);
					}
					else
					{
						ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12();
					}
				}
				break;
			}
			case 13:
			{
				{
					auto& _d13 = s_hotpatch_entries[13];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d13)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d13))
					{
						alignas(16) CHAOS_IL2CPP_UINT8 _d_ab[1];
						ArgBuffer _d_bw(_d_ab);
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d13.method_key, _d_ab, nullptr);
					}
					else
					{
						ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13();
					}
				}
				break;
			}
			default:
			{
				return;
				break;
			}
		}
	}
	return;
}



}  // namespace chaos::il2cpp::codegen::ThreadingMonitorInterlockedSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const CHAOS_IL2CPP_INT32 kAotMethodCount = 15;