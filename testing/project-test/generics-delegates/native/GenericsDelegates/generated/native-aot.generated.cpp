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

namespace chaos::il2cpp::codegen::GenericsDelegates {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_GenericsDelegates_BinaryOp[];
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);
inline TypeInfoV0 chaos_mt_GenericsDelegates_BinaryOp = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, chaos_vtable_GenericsDelegates_BinaryOp, 112572344463649328ULL, 4u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_GenericsDelegates_BinaryOp = static_cast<CHAOS_IL2CPP_INTPTR>(112572344463649328ULL);
inline TypeInfoV0 chaos_mt_GenericsDelegates_DelegateSubjects___O = {{nullptr, nullptr, 16418285889321439748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_GenericsDelegates_DelegateSubjects___O = static_cast<CHAOS_IL2CPP_INTPTR>(16418285889321439748ULL);
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
const void* chaos_vtable_GenericsDelegates_BinaryOp[] =
{
	nullptr,
	nullptr,
	nullptr,
	nullptr,
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_GenericsDelegates_BinaryOp[] =
{
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0u, nullptr },
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

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
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

struct chaos_type_GenericsDelegates_BinaryOp : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_GenericsDelegates_DelegateSubjects___O
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

CHAOS_IL2CPP_INTPTR chaos_static_GenericsDelegates_DelegateSubjects___O___0___Multiply = 0;


	// AOT-baked string table: sorted by StringId for binary search at runtime.
	constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
		{ 1628522870327039225U, "RTS_DEL_AS_PARAM=", 17u },
		{ 6462413471105097147U, "RTS_DEL_CUSTOM=", 15u },
		{ 6764446324755329143U, "RTS_DONE=ok", 11u },
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

extern "C" void GenericsDelegates_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void GenericsDelegates_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 GenericsDelegates_DelegateSubjects_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 GenericsDelegates_DelegateSubjects_ApplyFunction_BinaryOp_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 GenericsDelegates_DelegateSubjects_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" CHAOS_IL2CPP_INT32 GenericsDelegates_DelegateSubjects_TestCustomDelegate(void);
extern "C" CHAOS_IL2CPP_INT32 GenericsDelegates_DelegateSubjects_TestDelegateAsParam(void);
extern "C" void GenericsDelegates_Program__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void GenericsDelegates_Program_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[10] = {
	0u,
	0u,
	1u,
	3u,
	6u,
	8u,
	8u,
	8u,
	8u,
	9u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[9];
	CHAOS_IL2CPP_UINT8 params[9];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		9u,
		9u,
		2921675631u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // GenericsDelegates_AutoGeneratedProgram__ctor
		{ 0u, 1u },  // GenericsDelegates_AutoGeneratedProgram_Main_System_String
		{ 1u, 2u },  // GenericsDelegates_DelegateSubjects_Add_System_Int32_System_Int32
		{ 1u, 3u },  // GenericsDelegates_DelegateSubjects_ApplyFunction_BinaryOp_System_Int32_System_Int32
		{ 1u, 2u },  // GenericsDelegates_DelegateSubjects_Multiply_System_Int32_System_Int32
		{ 1u, 0u },  // GenericsDelegates_DelegateSubjects_TestCustomDelegate
		{ 1u, 0u },  // GenericsDelegates_DelegateSubjects_TestDelegateAsParam
		{ 0u, 0u },  // GenericsDelegates_Program__ctor
		{ 0u, 1u },  // GenericsDelegates_Program_Main_System_String
	},
	{
		2u,
		1u,
		1u,
		2u,
		1u,
		1u,
		1u,
		1u,
		2u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[6] = {
		0u,
		0u,
		0u,
		1544u,
		1548u,
		1548u,
	};

	static constexpr const char* s_type_names[6] = {
		"<Module>",
		"Program",
		"AutoGeneratedProgram",
		"BinaryOp",
		"DelegateSubjects",
		"<>O",
	};

	static constexpr const char* s_type_namespaces[6] = {
		"",
		"",
		"",
		"GenericsDelegates",
		"GenericsDelegates",
		"",
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

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[1] = {
		33554438u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[7] = {
		0u,
		0u,
		0u,
		0u,
		0u,
		1u,
		1u,
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
		/* .name_utf8         = */ "GenericsDelegates",
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
		/* .type_count        = */ 6u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("GenericsDelegates", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[9] = {
	{ ".ctor", 0x0000000Au, 0u },  // AutoGeneratedProgram
	{ "Main", 0x00000009u, 1u },  // AutoGeneratedProgram
	{ "Add", 0x0000000Fu, 2u },  // DelegateSubjects
	{ "ApplyFunction", 0x00000012u, 3u },  // DelegateSubjects
	{ "Multiply", 0x00000010u, 2u },  // DelegateSubjects
	{ "TestCustomDelegate", 0x00000011u, 0u },  // DelegateSubjects
	{ "TestDelegateAsParam", 0x00000013u, 0u },  // DelegateSubjects
	{ ".ctor", 0x00000008u, 0u },  // Program
	{ "<Main>$", 0x00000007u, 1u },  // Program
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[3] = {
	{ "AutoGeneratedProgram", "", 0u, 2u },
	{ "DelegateSubjects", "", 2u, 5u },
	{ "Program", "", 7u, 2u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[9] = {
	{ 0x00000007u, 8u },
	{ 0x00000008u, 7u },
	{ 0x00000009u, 1u },
	{ 0x0000000Au, 0u },
	{ 0x0000000Fu, 2u },
	{ 0x00000010u, 4u },
	{ 0x00000011u, 5u },
	{ 0x00000012u, 3u },
	{ 0x00000013u, 6u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[9] = {
	{ reinterpret_cast<void*>(&GenericsDelegates_AutoGeneratedProgram__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // AutoGeneratedProgram::.ctor
	{ reinterpret_cast<void*>(&GenericsDelegates_AutoGeneratedProgram_Main_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // AutoGeneratedProgram::Main
	{ reinterpret_cast<void*>(&GenericsDelegates_DelegateSubjects_Add_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // DelegateSubjects::Add
	{ reinterpret_cast<void*>(&GenericsDelegates_DelegateSubjects_ApplyFunction_BinaryOp_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // DelegateSubjects::ApplyFunction
	{ reinterpret_cast<void*>(&GenericsDelegates_DelegateSubjects_Multiply_System_Int32_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // DelegateSubjects::Multiply
	{ reinterpret_cast<void*>(&GenericsDelegates_DelegateSubjects_TestCustomDelegate), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // DelegateSubjects::TestCustomDelegate
	{ reinterpret_cast<void*>(&GenericsDelegates_DelegateSubjects_TestDelegateAsParam), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // DelegateSubjects::TestDelegateAsParam
	{ reinterpret_cast<void*>(&GenericsDelegates_Program__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // Program::.ctor
	{ reinterpret_cast<void*>(&GenericsDelegates_Program_Main_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // Program::<Main>$
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"GenericsDelegates",
	s_hotpatch_types,
	3u,
	s_hotpatch_methods,
	9u,
	s_hotpatch_slots,
	9u,
	s_hotpatch_entries,
	9u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[5] = {
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
	"GenericsDelegates/DelegateSubjects+<>O::<0>__Multiply",
	"System.Private.CoreLib/System.Int32::ToString:System.String()",
	"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)",
	"System.Console/System.Console::WriteLine:System.Void(System.String)",
};

extern "C" void* kChaosExternalRuntimeFnTable[5] = {
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Int32__ToString_System_String__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_),
};

extern "C" int32_t kChaosExternalRuntimeCount = 5;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[9])() = {
	reinterpret_cast<void(*)()>(&GenericsDelegates_AutoGeneratedProgram__ctor),
	reinterpret_cast<void(*)()>(&GenericsDelegates_AutoGeneratedProgram_Main_System_String),
	reinterpret_cast<void(*)()>(&GenericsDelegates_DelegateSubjects_Add_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&GenericsDelegates_DelegateSubjects_ApplyFunction_BinaryOp_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&GenericsDelegates_DelegateSubjects_Multiply_System_Int32_System_Int32),
	reinterpret_cast<void(*)()>(&GenericsDelegates_DelegateSubjects_TestCustomDelegate),
	reinterpret_cast<void(*)()>(&GenericsDelegates_DelegateSubjects_TestDelegateAsParam),
	reinterpret_cast<void(*)()>(&GenericsDelegates_Program__ctor),
	reinterpret_cast<void(*)()>(&GenericsDelegates_Program_Main_System_String),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[9])() = {
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[0])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[1])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[2])(0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[3])(0,0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[4])(0,0);},
	[]() {kAotMethods[5]();},
	[]() {kAotMethods[6]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[7])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[8])(0);},
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

	/* ── Entry 0: GenericsDelegates_AutoGeneratedProgram__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry0;
	/* ── Entry 1: GenericsDelegates_AutoGeneratedProgram_Main_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry1;
	/* ── Entry 2: GenericsDelegates_DelegateSubjects_Add_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry2;
	/* ── Entry 3: GenericsDelegates_DelegateSubjects_ApplyFunction_BinaryOp_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry3;
	/* ── Entry 4: GenericsDelegates_DelegateSubjects_Multiply_System_Int32_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry4;
	/* ── Entry 5: GenericsDelegates_DelegateSubjects_TestCustomDelegate ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry5;
	/* ── Entry 6: GenericsDelegates_DelegateSubjects_TestDelegateAsParam ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry6;
	/* ── Entry 7: GenericsDelegates_Program__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry7;
	/* ── Entry 8: GenericsDelegates_Program_Main_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry8;
} kChaosGcSlotMapsSection = {
	/* entry0 = GenericsDelegates_AutoGeneratedProgram__ctor */
	.entry0 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&GenericsDelegates_AutoGeneratedProgram__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry1 = GenericsDelegates_AutoGeneratedProgram_Main_System_String */
	.entry1 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&GenericsDelegates_AutoGeneratedProgram_Main_System_String),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry2 = GenericsDelegates_DelegateSubjects_Add_System_Int32_System_Int32 */
	.entry2 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&GenericsDelegates_DelegateSubjects_Add_System_Int32_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 16u, 24u }
	},
	/* entry3 = GenericsDelegates_DelegateSubjects_ApplyFunction_BinaryOp_System_Int32_System_Int32 */
	.entry3 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&GenericsDelegates_DelegateSubjects_ApplyFunction_BinaryOp_System_Int32_System_Int32),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 24u, 32u, 40u }
	},
	/* entry4 = GenericsDelegates_DelegateSubjects_Multiply_System_Int32_System_Int32 */
	.entry4 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&GenericsDelegates_DelegateSubjects_Multiply_System_Int32_System_Int32),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 16u, 24u }
	},
	/* entry5 = GenericsDelegates_DelegateSubjects_TestCustomDelegate */
	.entry5 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&GenericsDelegates_DelegateSubjects_TestCustomDelegate),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry6 = GenericsDelegates_DelegateSubjects_TestDelegateAsParam */
	.entry6 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&GenericsDelegates_DelegateSubjects_TestDelegateAsParam),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry7 = GenericsDelegates_Program__ctor */
	.entry7 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&GenericsDelegates_Program__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry8 = GenericsDelegates_Program_Main_System_String */
	.entry8 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&GenericsDelegates_Program_Main_System_String),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 276u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[9] = {
	reinterpret_cast<void*>(&GenericsDelegates_AutoGeneratedProgram__ctor),
	reinterpret_cast<void*>(&GenericsDelegates_AutoGeneratedProgram_Main_System_String),
	reinterpret_cast<void*>(&GenericsDelegates_DelegateSubjects_Add_System_Int32_System_Int32),
	reinterpret_cast<void*>(&GenericsDelegates_DelegateSubjects_ApplyFunction_BinaryOp_System_Int32_System_Int32),
	reinterpret_cast<void*>(&GenericsDelegates_DelegateSubjects_Multiply_System_Int32_System_Int32),
	reinterpret_cast<void*>(&GenericsDelegates_DelegateSubjects_TestCustomDelegate),
	reinterpret_cast<void*>(&GenericsDelegates_DelegateSubjects_TestDelegateAsParam),
	reinterpret_cast<void*>(&GenericsDelegates_Program__ctor),
	reinterpret_cast<void*>(&GenericsDelegates_Program_Main_System_String),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0x018FEFF36B49EA30),
		0x00000003u,
		0u,
		4u,
		reinterpret_cast<const void*>(kSlots_GenericsDelegates_BinaryOp),
		reinterpret_cast<const void**>(chaos_vtable_GenericsDelegates_BinaryOp),
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
	.method_pointer_count      = 9u,
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
	.image_name_utf8    = "GenericsDelegates",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryMethodDescriptor kReflMethods_GenericsDelegates_AutoGeneratedProgram[2] = {
	{ 0u, "GenericsDelegates/AutoGeneratedProgram::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "GenericsDelegates/AutoGeneratedProgram::Main:System.Void(System.String[])", "Main", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_GenericsDelegates_DelegateSubjects[5] = {
	{ 0u, "GenericsDelegates/DelegateSubjects::Add:System.Int32(System.Int32,System.Int32)", "Add", "System.Void", 0, nullptr, 0u },
	{ 0u, "GenericsDelegates/DelegateSubjects::ApplyFunction:System.Int32(BinaryOp,System.Int32,System.Int32)", "ApplyFunction", "System.Void", 0, nullptr, 0u },
	{ 0u, "GenericsDelegates/DelegateSubjects::Multiply:System.Int32(System.Int32,System.Int32)", "Multiply", "System.Void", 0, nullptr, 0u },
	{ 0u, "GenericsDelegates/DelegateSubjects::TestCustomDelegate:System.Int32()", "TestCustomDelegate", "System.Void", 0, nullptr, 0u },
	{ 0u, "GenericsDelegates/DelegateSubjects::TestDelegateAsParam:System.Int32()", "TestDelegateAsParam", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_GenericsDelegates_Program[2] = {
	{ 0u, "GenericsDelegates/Program::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "GenericsDelegates/Program::<Main>$:System.Void(System.String[])", "<Main>$", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[3] = {
	{ 0u, "GenericsDelegates/AutoGeneratedProgram", "GenericsDelegates/AutoGeneratedProgram", "", "AutoGeneratedProgram", "AutoGeneratedProgram", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_GenericsDelegates_AutoGeneratedProgram, 2u },
	{ 0u, "GenericsDelegates/DelegateSubjects", "GenericsDelegates/DelegateSubjects", "", "DelegateSubjects", "DelegateSubjects", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_GenericsDelegates_DelegateSubjects, 5u },
	{ 0u, "GenericsDelegates/Program", "GenericsDelegates/Program", "", "Program", "Program", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_GenericsDelegates_Program, 2u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[3] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
};

static const ReflectionQueryImageDescriptor kReflImage = { "GenericsDelegates", kReflTypePtrs, 3u };

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
// Managed method: GenericsDelegates/AutoGeneratedProgram::.ctor()
extern "C" void GenericsDelegates_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: GenericsDelegates/AutoGeneratedProgram::Main(System.String[])
extern "C" void GenericsDelegates_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: GenericsDelegates/DelegateSubjects::Add(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 GenericsDelegates_DelegateSubjects_Add_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapAdd(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GenericsDelegates/DelegateSubjects::ApplyFunction(BinaryOp,System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 GenericsDelegates_DelegateSubjects_ApplyFunction_BinaryOp_System_Int32_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INT32 chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);

	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s2 = chaos_args[2];
	{
		auto chaos_raw_arg_1 = _s2;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_delegate_value = _s0;
		if (chaos_delegate_value == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		auto* chaos_delegate = reinterpret_cast<chaos_type_GenericsDelegates_BinaryOp*>(chaos_delegate_value);
		if (chaos_delegate->chaos_delegate_invocation_count > 0)
		{
			const auto* chaos_invocation_list = reinterpret_cast<const CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_INTPTR)*>(chaos_delegate->chaos_delegate_invocation_list);
			if (chaos_invocation_list == nullptr ||
				static_cast<CHAOS_IL2CPP_INTPTR>(chaos_invocation_list->size()) != chaos_delegate->chaos_delegate_invocation_count)
			{
				CHAOS_IL2CPP_FAIL();
			}

			CHAOS_IL2CPP_INT32 chaos_result{};
			for (CHAOS_IL2CPP_SIZE chaos_delegate_index = 0; chaos_delegate_index < chaos_invocation_list->size(); ++chaos_delegate_index)
			{
				const auto chaos_invocation_delegate_value = (*chaos_invocation_list)[chaos_delegate_index];
				if (chaos_invocation_delegate_value == 0)
				{
					CHAOS_IL2CPP_FAIL();
				}
				auto* chaos_invocation_delegate = reinterpret_cast<chaos_type_GenericsDelegates_BinaryOp*>(chaos_invocation_delegate_value);
				if (chaos_invocation_delegate->chaos_delegate_method_ptr == 0)
				{
					CHAOS_IL2CPP_FAIL();
				}
				if (chaos_invocation_delegate->chaos_delegate_target == 0)
				{
					const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
					chaos_result = chaos_open_function(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				}
				else
				{
					const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target, CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
					chaos_result = chaos_closed_function(chaos_invocation_delegate->chaos_delegate_target, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				}
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		else
		{
			if (chaos_delegate->chaos_delegate_method_ptr == 0)
			{
				CHAOS_IL2CPP_FAIL();
			}
			CHAOS_IL2CPP_INT32 __chaos_hotpatch_result{};
			bool __chaos_hotpatch_taken = false;
			uint64_t __chaos_args_buf[2] = { (uint64_t)chaos_arg_0, (uint64_t)chaos_arg_1 };
			uint64_t __chaos_ret_buf[2] = {};
			if (chaos_delegate->chaos_delegate_method_token != 0)
			{
				__chaos_hotpatch_taken = ::chaos::il2cpp::runtime_core::DelegateHotpatchCheckpoint(
					chaos_delegate->chaos_delegate_method_token,
					__chaos_args_buf, __chaos_ret_buf, 2);
				if (__chaos_hotpatch_taken)
				{
					__chaos_hotpatch_result = *reinterpret_cast<const CHAOS_IL2CPP_INT32*>(__chaos_ret_buf);
				}
			}

			if (__chaos_hotpatch_taken)
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(__chaos_hotpatch_result);
			}
			else
			{
				if (chaos_delegate->chaos_delegate_target == 0)
				{
					const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)>(chaos_delegate->chaos_delegate_method_ptr);
					const auto chaos_result = chaos_open_function(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				else
				{
					const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target, CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)>(chaos_delegate->chaos_delegate_method_ptr);
					const auto chaos_result = chaos_closed_function(chaos_delegate->chaos_delegate_target, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
					_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
			}
		}
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s3);
}

// Managed method: GenericsDelegates/DelegateSubjects::Multiply(System.Int32,System.Int32)
extern "C" CHAOS_IL2CPP_INT32 GenericsDelegates_DelegateSubjects_Multiply_System_Int32_System_Int32(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(ChaosWrapMul(static_cast<CHAOS_IL2CPP_INT32>(_s0), static_cast<CHAOS_IL2CPP_INT32>(_s1)));
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: GenericsDelegates/DelegateSubjects::TestCustomDelegate()
extern "C" CHAOS_IL2CPP_INT32 GenericsDelegates_DelegateSubjects_TestCustomDelegate(void)
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


	_s0 = 0;
	{
		// Hotpatch-aware ldftn wrapper (slot 2)
		static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INT32 {
			auto& _d_entry = s_hotpatch_entries[2];
			if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
				&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
			{
				alignas(16) uint8_t _d_ab[8];
				ArgBuffer _d_bw(_d_ab);
				_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_0));
				_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
				CHAOS_IL2CPP_INT32 _d_ret{};
				::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
					_d_entry.method_key, _d_ab, &_d_ret);
				return _d_ret;
			}
			return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
		};
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
	}
	{
		const auto chaos_method_ptr = _s1;
		const auto chaos_target = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_GenericsDelegates_BinaryOp, {});
		chaos_object->header.type_info = &chaos_mt_GenericsDelegates_BinaryOp.hot;
		chaos_object->chaos_delegate_target = chaos_target;
		chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(3);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(4);
	{
		auto chaos_raw_arg_1 = _s2;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		auto chaos_raw_arg_0 = _s1;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_delegate_value = _s0;
		if (chaos_delegate_value == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		auto* chaos_delegate = reinterpret_cast<chaos_type_GenericsDelegates_BinaryOp*>(chaos_delegate_value);
		if (chaos_delegate->chaos_delegate_invocation_count > 0)
		{
			const auto* chaos_invocation_list = reinterpret_cast<const CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_INTPTR)*>(chaos_delegate->chaos_delegate_invocation_list);
			if (chaos_invocation_list == nullptr ||
				static_cast<CHAOS_IL2CPP_INTPTR>(chaos_invocation_list->size()) != chaos_delegate->chaos_delegate_invocation_count)
			{
				CHAOS_IL2CPP_FAIL();
			}

			CHAOS_IL2CPP_INT32 chaos_result{};
			for (CHAOS_IL2CPP_SIZE chaos_delegate_index = 0; chaos_delegate_index < chaos_invocation_list->size(); ++chaos_delegate_index)
			{
				const auto chaos_invocation_delegate_value = (*chaos_invocation_list)[chaos_delegate_index];
				if (chaos_invocation_delegate_value == 0)
				{
					CHAOS_IL2CPP_FAIL();
				}
				auto* chaos_invocation_delegate = reinterpret_cast<chaos_type_GenericsDelegates_BinaryOp*>(chaos_invocation_delegate_value);
				if (chaos_invocation_delegate->chaos_delegate_method_ptr == 0)
				{
					CHAOS_IL2CPP_FAIL();
				}
				if (chaos_invocation_delegate->chaos_delegate_target == 0)
				{
					const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
					chaos_result = chaos_open_function(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				}
				else
				{
					const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target, CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
					chaos_result = chaos_closed_function(chaos_invocation_delegate->chaos_delegate_target, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				}
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		else
		{
			if (chaos_delegate->chaos_delegate_method_ptr == 0)
			{
				CHAOS_IL2CPP_FAIL();
			}
			CHAOS_IL2CPP_INT32 __chaos_hotpatch_result{};
			bool __chaos_hotpatch_taken = false;
			uint64_t __chaos_args_buf[2] = { (uint64_t)chaos_arg_0, (uint64_t)chaos_arg_1 };
			uint64_t __chaos_ret_buf[2] = {};
			if (chaos_delegate->chaos_delegate_method_token != 0)
			{
				__chaos_hotpatch_taken = ::chaos::il2cpp::runtime_core::DelegateHotpatchCheckpoint(
					chaos_delegate->chaos_delegate_method_token,
					__chaos_args_buf, __chaos_ret_buf, 2);
				if (__chaos_hotpatch_taken)
				{
					__chaos_hotpatch_result = *reinterpret_cast<const CHAOS_IL2CPP_INT32*>(__chaos_ret_buf);
				}
			}

			if (__chaos_hotpatch_taken)
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(__chaos_hotpatch_result);
			}
			else
			{
				if (chaos_delegate->chaos_delegate_target == 0)
				{
					const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)>(chaos_delegate->chaos_delegate_method_ptr);
					const auto chaos_result = chaos_open_function(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				else
				{
					const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target, CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)>(chaos_delegate->chaos_delegate_method_ptr);
					const auto chaos_result = chaos_closed_function(chaos_delegate->chaos_delegate_target, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
					_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
			}
		}
	}
	return static_cast<CHAOS_IL2CPP_INT32>(_s3);
}

// Managed method: GenericsDelegates/DelegateSubjects::TestDelegateAsParam()
extern "C" CHAOS_IL2CPP_INT32 GenericsDelegates_DelegateSubjects_TestDelegateAsParam(void)
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


	_s0 = chaos_static_GenericsDelegates_DelegateSubjects___O___0___Multiply;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = 0;
			{
				// Hotpatch-aware ldftn wrapper (slot 4)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[4];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[8];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						CHAOS_IL2CPP_INT32 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s1;
				const auto chaos_target = _s0;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_GenericsDelegates_BinaryOp, {});
				chaos_object->header.type_info = &chaos_mt_GenericsDelegates_BinaryOp.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = _s0;
			{
				auto chaos_value = _s1;
				chaos_static_GenericsDelegates_DelegateSubjects___O___0___Multiply = chaos_value;
			}
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(6);
		{
			const auto chaos_arg_2 = _s2;
			const auto chaos_arg_1 = _s1;
			const auto chaos_raw_arg_0 = _s0;
			const auto chaos_arg_0 = chaos_normalize_native_int_argument(_s0);
			auto& _d3 = s_hotpatch_entries[3];
			CHAOS_IL2CPP_INT32 _d_hpresult{};
			if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d3)
				&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d3))
			{
				alignas(16) uint8_t _d_ab[16];
				ArgBuffer _d_bw(_d_ab);
				_d_bw.WritePtr(reinterpret_cast<void*>(chaos_arg_0));
				_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
				::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
					_d3.method_key, _d_ab, &_d_hpresult);
			}
			else
			{
				_d_hpresult = GenericsDelegates_DelegateSubjects_ApplyFunction_BinaryOp_System_Int32_System_Int32(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2));
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
		}
		return static_cast<CHAOS_IL2CPP_INT32>(_s0);
	}
}

// Managed method: GenericsDelegates/Program::.ctor()
extern "C" void GenericsDelegates_Program__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: GenericsDelegates/Program::<Main>$(System.String[])
extern "C" void GenericsDelegates_Program_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	{{
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_DEL_CUSTOM=");
	}}
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
			_d_hpresult = GenericsDelegates_DelegateSubjects_TestCustomDelegate();
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[0] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
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
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_DEL_AS_PARAM=");
	}}
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
			_d_hpresult = GenericsDelegates_DelegateSubjects_TestDelegateAsParam();
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(_d_hpresult);
	}
	chaos_locals[0] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
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
		_s0 = CHAOS_IL2CPP_STRING_ID("RTS_DONE=ok");
	}}
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Console_System_Console__WriteLine_System_Void_System_String_(chaos_arg_0);
	}
	return;
}



}  // namespace chaos::il2cpp::codegen::GenericsDelegates
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 9;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"generics-delegates.auto-generated-program.ctor\",\"subjectId\":\"GenericsDelegates/AutoGeneratedProgram::.ctor:System.Void()\",\"signature\":\"System.Void AutoGeneratedProgram::.ctor()\",\"identity\":{\"assemblyName\":\"GenericsDelegates\",\"declaringTypeSubjectId\":\"GenericsDelegates/AutoGeneratedProgram\",\"definitionSubjectId\":\"GenericsDelegates/AutoGeneratedProgram::.ctor:System.Void()\",\"subjectId\":\"GenericsDelegates/AutoGeneratedProgram::.ctor:System.Void()\",\"methodId\":\"generics-delegates.auto-generated-program.ctor\",\"signature\":\"System.Void AutoGeneratedProgram::.ctor()\"},\"nativeSymbol\":\"GenericsDelegates_AutoGeneratedProgram__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"GenericsDelegates/AutoGeneratedProgram\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenericsDelegates\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"generics-delegates.auto-generated-program.main\",\"subjectId\":\"GenericsDelegates/AutoGeneratedProgram::Main:System.Void(System.String[])\",\"signature\":\"System.Void AutoGeneratedProgram::Main(System.String[])\",\"identity\":{\"assemblyName\":\"GenericsDelegates\",\"declaringTypeSubjectId\":\"GenericsDelegates/AutoGeneratedProgram\",\"definitionSubjectId\":\"GenericsDelegates/AutoGeneratedProgram::Main:System.Void(System.String[])\",\"subjectId\":\"GenericsDelegates/AutoGeneratedProgram::Main:System.Void(System.String[])\",\"methodId\":\"generics-delegates.auto-generated-program.main\",\"signature\":\"System.Void AutoGeneratedProgram::Main(System.String[])\"},\"nativeSymbol\":\"GenericsDelegates_AutoGeneratedProgram_Main_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":1}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":0,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenericsDelegates\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"generics-delegates.delegate-subjects.add\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::Add:System.Int32(System.Int32,System.Int32)\",\"signature\":\"System.Int32 DelegateSubjects::Add(System.Int32,System.Int32)\",\"identity\":{\"assemblyName\":\"GenericsDelegates\",\"declaringTypeSubjectId\":\"GenericsDelegates/DelegateSubjects\",\"definitionSubjectId\":\"GenericsDelegates/DelegateSubjects::Add:System.Int32(System.Int32,System.Int32)\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::Add:System.Int32(System.Int32,System.Int32)\",\"methodId\":\"generics-delegates.delegate-subjects.add\",\"signature\":\"System.Int32 DelegateSubjects::Add(System.Int32,System.Int32)\"},\"nativeSymbol\":\"GenericsDelegates_DelegateSubjects_Add_System_Int32_System_Int32\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":3,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenericsDelegates\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"generics-delegates.delegate-subjects.apply-function\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::ApplyFunction:System.Int32(BinaryOp,System.Int32,System.Int32)\",\"signature\":\"System.Int32 DelegateSubjects::ApplyFunction(BinaryOp,System.Int32,System.Int32)\",\"identity\":{\"assemblyName\":\"GenericsDelegates\",\"declaringTypeSubjectId\":\"GenericsDelegates/DelegateSubjects\",\"definitionSubjectId\":\"GenericsDelegates/DelegateSubjects::ApplyFunction:System.Int32(BinaryOp,System.Int32,System.Int32)\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::ApplyFunction:System.Int32(BinaryOp,System.Int32,System.Int32)\",\"methodId\":\"generics-delegates.delegate-subjects.apply-function\",\"signature\":\"System.Int32 DelegateSubjects::ApplyFunction(BinaryOp,System.Int32,System.Int32)\"},\"nativeSymbol\":\"GenericsDelegates_DelegateSubjects_ApplyFunction_BinaryOp_System_Int32_System_Int32\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeSubjectId\":\"GenericsDelegates/BinaryOp\",\"typeShape\":1},{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"BinaryOp\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":2,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"callee\":\"GenericsDelegates/BinaryOp::Invoke:System.Int32(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"GenericsDelegates\",\"subjectKind\":\"method\",\"subjectId\":\"GenericsDelegates/BinaryOp::Invoke:System.Int32(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"GenericsDelegates\",\"subjectId\":\"GenericsDelegates/BinaryOp::Invoke:System.Int32(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"GenericsDelegates_BinaryOp_Invoke_System_Int32_System_Int32\",\"targetParameterCount\":2,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenericsDelegates\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"generics-delegates.delegate-subjects.multiply\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::Multiply:System.Int32(System.Int32,System.Int32)\",\"signature\":\"System.Int32 DelegateSubjects::Multiply(System.Int32,System.Int32)\",\"identity\":{\"assemblyName\":\"GenericsDelegates\",\"declaringTypeSubjectId\":\"GenericsDelegates/DelegateSubjects\",\"definitionSubjectId\":\"GenericsDelegates/DelegateSubjects::Multiply:System.Int32(System.Int32,System.Int32)\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::Multiply:System.Int32(System.Int32,System.Int32)\",\"methodId\":\"generics-delegates.delegate-subjects.multiply\",\"signature\":\"System.Int32 DelegateSubjects::Multiply(System.Int32,System.Int32)\"},\"nativeSymbol\":\"GenericsDelegates_DelegateSubjects_Multiply_System_Int32_System_Int32\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":1,\"typeShape\":2}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"mul\",\"opCode\":27,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":3,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenericsDelegates\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"generics-delegates.delegate-subjects.test-custom-delegate\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::TestCustomDelegate:System.Int32()\",\"signature\":\"System.Int32 DelegateSubjects::TestCustomDelegate()\",\"identity\":{\"assemblyName\":\"GenericsDelegates\",\"declaringTypeSubjectId\":\"GenericsDelegates/DelegateSubjects\",\"definitionSubjectId\":\"GenericsDelegates/DelegateSubjects::TestCustomDelegate:System.Int32()\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::TestCustomDelegate:System.Int32()\",\"methodId\":\"generics-delegates.delegate-subjects.test-custom-delegate\",\"signature\":\"System.Int32 DelegateSubjects::TestCustomDelegate()\"},\"nativeSymbol\":\"GenericsDelegates_DelegateSubjects_TestCustomDelegate\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":0,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldftn\",\"opCode\":73,\"operand\":\"GenericsDelegates/DelegateSubjects::Add:System.Int32(System.Int32,System.Int32)\",\"ilOffset\":1,\"resultType\":\"System.IntPtr\",\"callee\":\"GenericsDelegates/DelegateSubjects::Add:System.Int32(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"GenericsDelegates\",\"subjectKind\":\"method\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::Add:System.Int32(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"GenericsDelegates\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::Add:System.Int32(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":7,\"resultType\":\"GenericsDelegates/BinaryOp\",\"callee\":\"GenericsDelegates/BinaryOp::.ctor:System.Void(System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"GenericsDelegates\",\"subjectKind\":\"method\",\"subjectId\":\"GenericsDelegates/BinaryOp::.ctor:System.Void(System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"GenericsDelegates\",\"subjectId\":\"GenericsDelegates/BinaryOp\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":true,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.MulticastDelegate\",\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":3,\"ilOffset\":14,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":4,\"ilOffset\":15,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"callee\":\"GenericsDelegates/BinaryOp::Invoke:System.Int32(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"GenericsDelegates\",\"subjectKind\":\"method\",\"subjectId\":\"GenericsDelegates/BinaryOp::Invoke:System.Int32(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"GenericsDelegates\",\"subjectId\":\"GenericsDelegates/BinaryOp::Invoke:System.Int32(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"GenericsDelegates_BinaryOp_Invoke_System_Int32_System_Int32\",\"targetParameterCount\":2,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":2,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":21,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenericsDelegates\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"generics-delegates.delegate-subjects.test-delegate-as-param\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::TestDelegateAsParam:System.Int32()\",\"signature\":\"System.Int32 DelegateSubjects::TestDelegateAsParam()\",\"identity\":{\"assemblyName\":\"GenericsDelegates\",\"declaringTypeSubjectId\":\"GenericsDelegates/DelegateSubjects\",\"definitionSubjectId\":\"GenericsDelegates/DelegateSubjects::TestDelegateAsParam:System.Int32()\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::TestDelegateAsParam:System.Int32()\",\"methodId\":\"generics-delegates.delegate-subjects.test-delegate-as-param\",\"signature\":\"System.Int32 DelegateSubjects::TestDelegateAsParam()\"},\"nativeSymbol\":\"GenericsDelegates_DelegateSubjects_TestDelegateAsParam\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"GenericsDelegates/DelegateSubjects\\u002B\\u003C\\u003EO::\\u003C0\\u003E__Multiply\",\"ilOffset\":0,\"resultType\":\"BinaryOp\",\"reference\":{\"assemblyName\":\"GenericsDelegates\",\"subjectKind\":\"field\",\"subjectId\":\"GenericsDelegates/DelegateSubjects\\u002B\\u003C\\u003EO::\\u003C0\\u003E__Multiply\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"GenericsDelegates\",\"subjectId\":\"GenericsDelegates/DelegateSubjects\\u002B\\u003C\\u003EO::\\u003C0\\u003E__Multiply\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"GenericsDelegates/DelegateSubjects\\u002B\\u003C\\u003EO\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"BinaryOp\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":5,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":27,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":9,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldftn\",\"opCode\":73,\"operand\":\"GenericsDelegates/DelegateSubjects::Multiply:System.Int32(System.Int32,System.Int32)\",\"ilOffset\":10,\"resultType\":\"System.IntPtr\",\"callee\":\"GenericsDelegates/DelegateSubjects::Multiply:System.Int32(System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"GenericsDelegates\",\"subjectKind\":\"method\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::Multiply:System.Int32(System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"GenericsDelegates\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::Multiply:System.Int32(System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":16,\"resultType\":\"GenericsDelegates/BinaryOp\",\"callee\":\"GenericsDelegates/BinaryOp::.ctor:System.Void(System.Object,System.IntPtr)\",\"reference\":{\"assemblyName\":\"GenericsDelegates\",\"subjectKind\":\"method\",\"subjectId\":\"GenericsDelegates/BinaryOp::.ctor:System.Void(System.Object,System.IntPtr)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"GenericsDelegates\",\"subjectId\":\"GenericsDelegates/BinaryOp\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":true,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.MulticastDelegate\",\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"GenericsDelegates/DelegateSubjects\\u002B\\u003C\\u003EO::\\u003C0\\u003E__Multiply\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"GenericsDelegates\",\"subjectKind\":\"field\",\"subjectId\":\"GenericsDelegates/DelegateSubjects\\u002B\\u003C\\u003EO::\\u003C0\\u003E__Multiply\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"GenericsDelegates\",\"subjectId\":\"GenericsDelegates/DelegateSubjects\\u002B\\u003C\\u003EO::\\u003C0\\u003E__Multiply\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"GenericsDelegates/DelegateSubjects\\u002B\\u003C\\u003EO\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"BinaryOp\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":5,\"ilOffset\":27,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":6,\"ilOffset\":28,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":29,\"resultType\":\"System.Int32\",\"callee\":\"GenericsDelegates/DelegateSubjects::ApplyFunction:System.Int32(BinaryOp,System.Int32,System.Int32)\",\"reference\":{\"assemblyName\":\"GenericsDelegates\",\"subjectKind\":\"method\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::ApplyFunction:System.Int32(BinaryOp,System.Int32,System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"GenericsDelegates\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::ApplyFunction:System.Int32(BinaryOp,System.Int32,System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"GenericsDelegates_DelegateSubjects_ApplyFunction_BinaryOp_System_Int32_System_Int32\",\"targetParameterCount\":3,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":34,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenericsDelegates\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"generics-delegates.program.ctor\",\"subjectId\":\"GenericsDelegates/Program::.ctor:System.Void()\",\"signature\":\"System.Void Program::.ctor()\",\"identity\":{\"assemblyName\":\"GenericsDelegates\",\"declaringTypeSubjectId\":\"GenericsDelegates/Program\",\"definitionSubjectId\":\"GenericsDelegates/Program::.ctor:System.Void()\",\"subjectId\":\"GenericsDelegates/Program::.ctor:System.Void()\",\"methodId\":\"generics-delegates.program.ctor\",\"signature\":\"System.Void Program::.ctor()\"},\"nativeSymbol\":\"GenericsDelegates_Program__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"GenericsDelegates/Program\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenericsDelegates\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"generics-delegates.program.main\",\"subjectId\":\"GenericsDelegates/Program::\\u003CMain\\u003E$:System.Void(System.String[])\",\"signature\":\"System.Void Program::\\u003CMain\\u003E$(System.String[])\",\"identity\":{\"assemblyName\":\"GenericsDelegates\",\"declaringTypeSubjectId\":\"GenericsDelegates/Program\",\"definitionSubjectId\":\"GenericsDelegates/Program::\\u003CMain\\u003E$:System.Void(System.String[])\",\"subjectId\":\"GenericsDelegates/Program::\\u003CMain\\u003E$:System.Void(System.String[])\",\"methodId\":\"generics-delegates.program.main\",\"signature\":\"System.Void Program::\\u003CMain\\u003E$(System.String[])\"},\"nativeSymbol\":\"GenericsDelegates_Program_Main_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_DEL_CUSTOM=\",\"ilOffset\":0,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"callee\":\"GenericsDelegates/DelegateSubjects::TestCustomDelegate:System.Int32()\",\"reference\":{\"assemblyName\":\"GenericsDelegates\",\"subjectKind\":\"method\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::TestCustomDelegate:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"GenericsDelegates\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::TestCustomDelegate:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"GenericsDelegates_DelegateSubjects_TestCustomDelegate\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":11,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":13,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":18,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":23,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_DEL_AS_PARAM=\",\"ilOffset\":28,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"callee\":\"GenericsDelegates/DelegateSubjects::TestDelegateAsParam:System.Int32()\",\"reference\":{\"assemblyName\":\"GenericsDelegates\",\"subjectKind\":\"method\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::TestDelegateAsParam:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"GenericsDelegates\",\"subjectId\":\"GenericsDelegates/DelegateSubjects::TestDelegateAsParam:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"targetSymbol\":\"GenericsDelegates_DelegateSubjects_TestDelegateAsParam\",\"targetParameterCount\":0,\"targetReturnType\":\"System.Int32\",\"dispatchKindCode\":1,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":38,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":39,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":41,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Int32::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":46,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":51,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"RTS_DONE=ok\",\"ilOffset\":56,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":61,\"resultType\":\"System.Void\",\"callee\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":66,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"GenericsDelegates\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[9] =
{
    { kMethodJson_0, 1883u, 0x0000000Au, 0u },
    { kMethodJson_1, 1280u, 0x00000009u, 0u },
    { kMethodJson_2, 1628u, 0x0000000Fu, 0u },
    { kMethodJson_3, 2629u, 0x00000012u, 0u },
    { kMethodJson_4, 1668u, 0x00000010u, 0u },
    { kMethodJson_5, 3922u, 0x00000011u, 0u },
    { kMethodJson_6, 5679u, 0x00000013u, 0u },
    { kMethodJson_7, 1749u, 0x00000008u, 0u },
    { kMethodJson_8, 7908u, 0x00000007u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 9u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}