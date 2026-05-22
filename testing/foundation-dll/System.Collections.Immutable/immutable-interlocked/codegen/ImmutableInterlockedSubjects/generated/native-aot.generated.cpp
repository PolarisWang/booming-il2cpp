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

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

namespace chaos::il2cpp::codegen::ImmutableInterlockedSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c[];
inline TypeInfoV0 chaos_mt_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects = {{nullptr, nullptr, 6608399795421352936ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(6608399795421352936ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c, 14505324169980688848ULL, 2u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c = static_cast<CHAOS_IL2CPP_INTPTR>(14505324169980688848ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_String_System_String_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 4000746079747307385ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_String_System_String_ = static_cast<CHAOS_IL2CPP_INTPTR>(4000746079747307385ULL);
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

// ── Virtual method table arrays ──
extern "C" void ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INTPTR ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_CustomEntrySubject_0_b__1_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
const void* chaos_vtable_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c[] =
{
	reinterpret_cast<void*>(ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c__ctor),
	reinterpret_cast<void*>(ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_CustomEntrySubject_0_b__1_0_System_String),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c[] =
{
	{ 0x00000008u, reinterpret_cast<void*>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c__ctor) },
	{ 0x00000009u, reinterpret_cast<void*>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_CustomEntrySubject_0_b__1_0_System_String) },
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

struct chaos_type_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_System_Private_CoreLib_System_Delegate : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR chaos_delegate_target = 0;
	CHAOS_IL2CPP_INTPTR chaos_delegate_method_ptr = 0;
	CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_list = 0;
	CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_count = 0;
	CHAOS_IL2CPP_UINT32 chaos_delegate_method_token = 0;
	CHAOS_IL2CPP_UINT32 _pad = 0;
};

struct chaos_type_System_Private_CoreLib_System_MulticastDelegate : public chaos_type_System_Private_CoreLib_System_Delegate
{
};

struct chaos_type_System_Private_CoreLib_System_Func_System_String_System_String_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
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

CHAOS_IL2CPP_INTPTR chaos_static_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c____9__1_0 = 0;
CHAOS_IL2CPP_INT32 chaos_static_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___exitCode = 0;


	// AOT-baked string table: sorted by StringId for binary search at runtime.
	constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
		{ 2607821981565500683U, "hello", 5u },
		{ 6500971301520520093U, " world", 6u },
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

static void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	return ChaosReflectionConcatStringPairValues(chaos_fn_arg_0, chaos_fn_arg_1);
}

CHAOS_IL2CPP_ONCE_FLAG chaos_type_init_once_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c;

void chaos_ensure_type_initialized_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c()
{
	CHAOS_IL2CPP_CALL_ONCE(chaos_type_init_once_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c, []()
	{
	if (chaos_static_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c____9 == 0)
	{
	    auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c);
	    chaos_object->header.type_info = &chaos_mt_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c.hot;
		const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
		ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c__ctor(chaos_arg_0);
	    chaos_static_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c____9 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	});
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void ImmutableInterlockedSubjects_ImmutableInterlockedSubjects_CustomEntrySubject_0(void);
extern "C" CHAOS_IL2CPP_INTPTR ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_CustomEntrySubject_0_b__1_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_cctor(void);
extern "C" void ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[5] = {
	0u,
	0u,
	1u,
	1u,
	1u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[4];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		4u,
		1u,
		926566178u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ImmutableInterlockedSubjects_ImmutableInterlockedSubjects_CustomEntrySubject_0
		{ 2u, 1u },  // ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_CustomEntrySubject_0_b__1_0_System_String
		{ 0u, 0u },  // ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_cctor
		{ 0u, 0u },  // ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c__ctor
	},
	{
		2u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[3] = {
		0u,
		1548u,
		1544u,
	};

	static constexpr const char* s_type_names[3] = {
		"<Module>",
		"ImmutableInterlockedSubjects",
		"<>c",
	};

	static constexpr const char* s_type_namespaces[3] = {
		"",
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[3] = {
		0u,
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[3] = {
		nullptr,
		&chaos_mt_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects.hot,
		&chaos_mt_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c.hot,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[1] = {
		33554435u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[4] = {
		0u,
		0u,
		1u,
		1u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[4] = {
		0u,
		0u,
		0u,
		0u,
	};



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "ImmutableInterlockedSubjects",
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
		/* .type_count        = */ 3u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("ImmutableInterlockedSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[4] = {
	{ "CustomEntrySubject_0", 0x00000006u, 0u },  // ImmutableInterlockedSubjects
	{ "<CustomEntrySubject_0>b__1_0", 0x00000009u, 1u },  // ImmutableInterlockedSubjects+<>c
	{ ".cctor", 0x00000007u, 0u },  // ImmutableInterlockedSubjects+<>c
	{ ".ctor", 0x00000008u, 0u },  // ImmutableInterlockedSubjects+<>c
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "ImmutableInterlockedSubjects", "", 0u, 1u },
	{ "ImmutableInterlockedSubjects+<>c", "", 1u, 3u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[4] = {
	{ 0x00000006u, 0u },
	{ 0x00000007u, 2u },
	{ 0x00000008u, 3u },
	{ 0x00000009u, 1u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[4] = {
	{ reinterpret_cast<void*>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImmutableInterlockedSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_CustomEntrySubject_0_b__1_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImmutableInterlockedSubjects+<>c::<CustomEntrySubject_0>b__1_0
	{ reinterpret_cast<void*>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_cctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImmutableInterlockedSubjects+<>c::.cctor
	{ reinterpret_cast<void*>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // ImmutableInterlockedSubjects+<>c::.ctor
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ImmutableInterlockedSubjects",
	s_hotpatch_types,
	2u,
	s_hotpatch_methods,
	4u,
	s_hotpatch_slots,
	4u,
	s_hotpatch_entries,
	4u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[7] = {
	"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects+<>c::<>9__1_0",
	"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects+<>c::<>9",
	"System.Private.CoreLib/System.Func<System.String,System.String>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Collections.Immutable/ImmutableInterlocked::Update<System.String>:System.Boolean(System.String&,System.Func<System.String,System.String>)",
	"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects::_exitCode",
	"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
};

extern "C" void* kChaosExternalRuntimeFnTable[7] = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 7;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[4])() = {
	reinterpret_cast<void(*)()>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects_CustomEntrySubject_0),
	reinterpret_cast<void(*)()>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_CustomEntrySubject_0_b__1_0_System_String),
	reinterpret_cast<void(*)()>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_cctor),
	reinterpret_cast<void(*)()>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c__ctor),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[4])() = {
	[]() {kAotMethods[0]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[1])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),chaos_make_string_id_value(6500971301520520093ULL));},
	[]() {kAotMethods[2]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[3])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 1;
extern "C" const int kSubjectEntryIndices[1] = {
	0,
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

	/* ── Entry 0: ImmutableInterlockedSubjects_ImmutableInterlockedSubjects_CustomEntrySubject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry0;
	/* ── Entry 1: ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_CustomEntrySubject_0_b__1_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry1;
	/* ── Entry 2: ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_cctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry2;
	/* ── Entry 3: ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry3;
} kChaosGcSlotMapsSection = {
	/* entry0 = ImmutableInterlockedSubjects_ImmutableInterlockedSubjects_CustomEntrySubject_0 */
	.entry0 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects_CustomEntrySubject_0),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry1 = ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_CustomEntrySubject_0_b__1_0_System_String */
	.entry1 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_CustomEntrySubject_0_b__1_0_System_String),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry2 = ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_cctor */
	.entry2 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_cctor),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry3 = ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c__ctor */
	.entry3 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 132u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[4] = {
	reinterpret_cast<void*>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects_CustomEntrySubject_0),
	reinterpret_cast<void*>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_CustomEntrySubject_0_b__1_0_System_String),
	reinterpret_cast<void*>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_cctor),
	reinterpret_cast<void*>(&ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c__ctor),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0xC94D437AE79639D0),
		0x00000002u,
		0u,
		2u,
		reinterpret_cast<const void*>(kSlots_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c),
		reinterpret_cast<const void**>(chaos_vtable_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c),
		2u,
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
	.method_pointer_count      = 4u,
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
	.image_name_utf8    = "ImmutableInterlockedSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects[1] = {
	{ 0u, "ImmutableInterlockedSubjects/ImmutableInterlockedSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects[1] = {
	{ 0u, "ImmutableInterlockedSubjects/ImmutableInterlockedSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c[3] = {
	{ 0u, "ImmutableInterlockedSubjects/ImmutableInterlockedSubjects+<>c::<CustomEntrySubject_0>b__1_0:System.String(System.String)", "<CustomEntrySubject_0>b__1_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ImmutableInterlockedSubjects/ImmutableInterlockedSubjects+<>c::.cctor:System.Void()", ".cctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "ImmutableInterlockedSubjects/ImmutableInterlockedSubjects+<>c::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0u, "ImmutableInterlockedSubjects/ImmutableInterlockedSubjects", "ImmutableInterlockedSubjects/ImmutableInterlockedSubjects", "", "ImmutableInterlockedSubjects", "ImmutableInterlockedSubjects", nullptr, kReflFields_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects, 1u, nullptr, 0u,
	kReflMethods_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects, 1u },
	{ 0u, "ImmutableInterlockedSubjects/ImmutableInterlockedSubjects+<>c", "ImmutableInterlockedSubjects/ImmutableInterlockedSubjects+<>c", "", "ImmutableInterlockedSubjects+<>c", "ImmutableInterlockedSubjects+<>c", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c, 3u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ImmutableInterlockedSubjects", kReflTypePtrs, 2u };

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
// Managed method: ImmutableInterlockedSubjects/ImmutableInterlockedSubjects::CustomEntrySubject_0()
extern "C" void ImmutableInterlockedSubjects_ImmutableInterlockedSubjects_CustomEntrySubject_0(void)
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


	CHAOS_EH_TRY
			{{
				_s0 = CHAOS_IL2CPP_STRING_ID("hello");
			}}
			chaos_locals[0] = _s0;
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
			chaos_ensure_type_initialized_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c();
			_s1 = chaos_static_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c____9__1_0;
			_s2 = _s1;
			// brtrue (structured EH branch)
			chaos_ensure_type_initialized_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c();
			_s2 = chaos_static_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 1)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> CHAOS_IL2CPP_INTPTR {
					auto& _d_entry = s_hotpatch_entries[1];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[16];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INTPTR _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s3;
				const auto chaos_target = _s2;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_String_System_String_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_String_System_String_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s3 = _s2;
			{
				auto chaos_value = _s3;
			chaos_ensure_type_initialized_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c();
				chaos_static_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c____9__1_0 = chaos_value;
			}
			{
				const auto chaos_arg_1 = _s2;
				const auto chaos_arg_0 = _s1;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_arg_0, chaos_arg_1);
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s1;
			_s1 = chaos_locals[1];
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
			chaos_locals[2] = _s1;
			_s1 = chaos_locals[2];
			// brfalse (structured EH branch)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_Object.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ImmutableInterlockedSubjects/ImmutableInterlockedSubjects+<>c::<CustomEntrySubject_0>b__1_0(System.String)
extern "C" CHAOS_IL2CPP_INTPTR ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_CustomEntrySubject_0_b__1_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[1];
	{{
		_s1 = CHAOS_IL2CPP_STRING_ID(" world");
	}}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return _s0;
}

// Managed method: ImmutableInterlockedSubjects/ImmutableInterlockedSubjects+<>c::.cctor()
extern "C" void ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_cctor(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};

	chaos_ensure_type_initialized_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c();

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c, {});
		chaos_object->header.type_info = &chaos_mt_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c.hot;
		ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
	chaos_ensure_type_initialized_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c();
		chaos_static_ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c____9 = chaos_value;
	}
	return;
}

// Managed method: ImmutableInterlockedSubjects/ImmutableInterlockedSubjects+<>c::.ctor()
extern "C" void ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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



}  // namespace chaos::il2cpp::codegen::ImmutableInterlockedSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 4;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"immutable-interlocked-subjects.immutable-interlocked-subjects.custom-entry-subject-0\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects::CustomEntrySubject_0:System.Void()\",\"signature\":\"System.Void ImmutableInterlockedSubjects::CustomEntrySubject_0()\",\"identity\":{\"assemblyName\":\"ImmutableInterlockedSubjects\",\"declaringTypeSubjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\",\"definitionSubjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects::CustomEntrySubject_0:System.Void()\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects::CustomEntrySubject_0:System.Void()\",\"methodId\":\"immutable-interlocked-subjects.immutable-interlocked-subjects.custom-entry-subject-0\",\"signature\":\"System.Void ImmutableInterlockedSubjects::CustomEntrySubject_0()\"},\"nativeSymbol\":\"ImmutableInterlockedSubjects_ImmutableInterlockedSubjects_CustomEntrySubject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":1,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":63,\"handlerOffset\":64,\"handlerLength\":11,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"hello\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__1_0\",\"ilOffset\":10,\"resultType\":\"System.Func\\u003CSystem.String,System.String\\u003E\",\"reference\":{\"assemblyName\":\"ImmutableInterlockedSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__1_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ImmutableInterlockedSubjects\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__1_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.String,System.String\\u003E\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":15,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":41,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":18,\"isPreserveSig\":false},{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"ilOffset\":19,\"resultType\":\"ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec\",\"reference\":{\"assemblyName\":\"ImmutableInterlockedSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ImmutableInterlockedSubjects\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldftn\",\"opCode\":73,\"operand\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_0\\u003Eb__1_0:System.String(System.String)\",\"ilOffset\":24,\"resultType\":\"System.IntPtr\",\"callee\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_0\\u003Eb__1_0:System.String(System.String)\",\"reference\":{\"assemblyName\":\"ImmutableInterlockedSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_0\\u003Eb__1_0:System.String(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"ImmutableInterlockedSubjects\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_0\\u003Eb__1_0:System.String(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":30,\"resultType\":\"System.Private.CoreLib/System.Func\\u003CSystem.String,System.String\\u003E\",\"callee\":\"System.Private.CoreLib/System.Func\\u003CSystem.String,System.String\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.String,System.String\\u003E::.ctor:System.Void(System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Func\\u003CSystem.String,System.String\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":35,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__1_0\",\"ilOffset\":36,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ImmutableInterlockedSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__1_0\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ImmutableInterlockedSubjects\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9__1_0\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Func\\u003CSystem.String,System.String\\u003E\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":41,\"resultType\":\"System.Boolean\",\"callee\":\"System.Collections.Immutable/ImmutableInterlocked::Update\\u003CSystem.String\\u003E:System.Boolean(System.String\\u0026,System.Func\\u003CSystem.String,System.String\\u003E)\",\"reference\":{\"assemblyName\":\"System.Collections.Immutable\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections.Immutable/ImmutableInterlocked::Update\\u003CSystem.String\\u003E:System.Boolean(System.String\\u0026,System.Func\\u003CSystem.String,System.String\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections.Immutable\",\"subjectId\":\"System.Collections.Immutable/ImmutableInterlocked::Update\\u003CSystem.String\\u003E:System.Boolean(System.String\\u0026,System.Func\\u003CSystem.String,System.String\\u003E)\",\"openDefinitionSubjectId\":\"System.Collections.Immutable/ImmutableInterlocked::Update:System.Boolean(!!0\\u0026,System.Func\\u003C!!0,!!0\\u003E)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableInterlocked::Update:System.Boolean(!!0\\u0026,System.Func\\u003C!!0,!!0\\u003E);type=[];method=[System.String]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableInterlocked::Update:System.Boolean(!!0\\u0026,System.Func\\u003C!!0,!!0\\u003E);type=[];method=[System.String]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableInterlocked::Update:System.Boolean(!!0\\u0026,System.Func\\u003C!!0,!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.String\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections.Immutable/ImmutableInterlocked::Update:System.Boolean(!!0\\u0026,System.Func\\u003C!!0,!!0\\u003E);type=[];method=[System.String]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections.Immutable/ImmutableInterlocked::Update:System.Boolean(!!0\\u0026,System.Func\\u003C!!0,!!0\\u003E);type=[];method=[System.String]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections.Immutable/ImmutableInterlocked::Update\\u003CSystem.String\\u003E:System.Boolean(System.String\\u0026,System.Func\\u003CSystem.String,System.String\\u003E)\",\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableInterlocked::Update:System.Boolean(!!0\\u0026,System.Func\\u003C!!0,!!0\\u003E)\",\"displaySubjectId\":\"System.Collections.Immutable/ImmutableInterlocked::Update\\u003CSystem.String\\u003E(System.String\\u0026,System.Func\\u003CSystem.String,System.String\\u003E)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Collections.Immutable/ImmutableInterlocked::Update:System.Boolean(!!0\\u0026,System.Func\\u003C!!0,!!0\\u003E)\",\"typeArguments\":[],\"methodArguments\":[\"System.String\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":46,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":47,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":48,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":49,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":52,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":61,\"ilOffset\":53,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":55,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects::_exitCode\",\"ilOffset\":56,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ImmutableInterlockedSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ImmutableInterlockedSubjects\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":75,\"ilOffset\":62,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":64,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":66,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects::_exitCode\",\"ilOffset\":67,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ImmutableInterlockedSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects::_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ImmutableInterlockedSubjects\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects::_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":75,\"ilOffset\":73,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":75,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ImmutableInterlockedSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"immutable-interlocked-subjects.immutable-interlocked-subjects-c.custom-entry-subject-0-b-1-0\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_0\\u003Eb__1_0:System.String(System.String)\",\"signature\":\"System.String ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_0\\u003Eb__1_0(System.String)\",\"identity\":{\"assemblyName\":\"ImmutableInterlockedSubjects\",\"declaringTypeSubjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec\",\"definitionSubjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_0\\u003Eb__1_0:System.String(System.String)\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_0\\u003Eb__1_0:System.String(System.String)\",\"methodId\":\"immutable-interlocked-subjects.immutable-interlocked-subjects-c.custom-entry-subject-0-b-1-0\",\"signature\":\"System.String ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003CCustomEntrySubject_0\\u003Eb__1_0(System.String)\"},\"nativeSymbol\":\"ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_CustomEntrySubject_0_b__1_0_System_String\",\"isStatic\":false,\"returnType\":\"System.String\",\"returnAbi\":{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\" world\",\"ilOffset\":1,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":6,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":11,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ImmutableInterlockedSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"immutable-interlocked-subjects.immutable-interlocked-subjects-c.cctor\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::.cctor:System.Void()\",\"signature\":\"System.Void ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::.cctor()\",\"identity\":{\"assemblyName\":\"ImmutableInterlockedSubjects\",\"declaringTypeSubjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec\",\"definitionSubjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::.cctor:System.Void()\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::.cctor:System.Void()\",\"methodId\":\"immutable-interlocked-subjects.immutable-interlocked-subjects-c.cctor\",\"signature\":\"System.Void ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::.cctor()\"},\"nativeSymbol\":\"ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c_cctor\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":0,\"resultType\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec\",\"callee\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"ImmutableInterlockedSubjects\",\"subjectKind\":\"method\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::.ctor:System.Void()\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"ImmutableInterlockedSubjects\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":true,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Object\",\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"ilOffset\":5,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"ImmutableInterlockedSubjects\",\"subjectKind\":\"field\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"ImmutableInterlockedSubjects\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::\\u003C\\u003E9\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ImmutableInterlockedSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"immutable-interlocked-subjects.immutable-interlocked-subjects-c.ctor\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::.ctor:System.Void()\",\"signature\":\"System.Void ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::.ctor()\",\"identity\":{\"assemblyName\":\"ImmutableInterlockedSubjects\",\"declaringTypeSubjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec\",\"definitionSubjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::.ctor:System.Void()\",\"subjectId\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::.ctor:System.Void()\",\"methodId\":\"immutable-interlocked-subjects.immutable-interlocked-subjects-c.ctor\",\"signature\":\"System.Void ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec::.ctor()\"},\"nativeSymbol\":\"ImmutableInterlockedSubjects_ImmutableInterlockedSubjects___c__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"ImmutableInterlockedSubjects/ImmutableInterlockedSubjects\\u002B\\u003C\\u003Ec\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"ImmutableInterlockedSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[4] =
{
    { kMethodJson_0, 12418u, 0x00000006u, 0u },
    { kMethodJson_1, 2826u, 0x00000009u, 0u },
    { kMethodJson_2, 3218u, 0x00000007u, 0u },
    { kMethodJson_3, 2216u, 0x00000008u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 4u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}