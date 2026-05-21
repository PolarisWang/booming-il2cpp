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

namespace chaos::il2cpp::codegen::ThreadingMonitorInterlockedSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c[];
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Func_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 8866194404114377402ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Func_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(8866194404114377402ULL);
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
inline TypeInfoV0 chaos_mt_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects = {{nullptr, nullptr, 4882207410565625576ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(4882207410565625576ULL);
inline TypeInfoV0 chaos_mt_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c, 9387555369835203792ULL, 3u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c = static_cast<CHAOS_IL2CPP_INTPTR>(9387555369835203792ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Int32 = {{nullptr, nullptr, 11009693519287992193ULL, 0u, 32, 2, 0}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_boxed_type_id_System_Private_CoreLib_System_Int32 = static_cast<CHAOS_IL2CPP_INTPTR>(11009693519287992193ULL);

// ── Virtual method table arrays ──
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c[] =
{
	reinterpret_cast<void*>(ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c__ctor),
	reinterpret_cast<void*>(ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_0),
	reinterpret_cast<void*>(ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_1),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c[] =
{
	{ 0x00000019u, reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c__ctor) },
	{ 0x0000001Au, reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_0) },
	{ 0x0000001Bu, reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_1) },
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

struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
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

struct chaos_type_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_boxed_type_System_Private_CoreLib_System_Int32
{
	PureTypeHeader header{};
	CHAOS_IL2CPP_INTPTR value = 0;
};

CHAOS_IL2CPP_INTPTR chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c____9__14_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c____9__14_1 = 0;
CHAOS_IL2CPP_INT32 chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = 0;

static void chaos_external_runtime_System_Private_CoreLib_Interlocked__MemoryBarrier_System_Void__(void)
{
	ChaosInterlockedMemoryBarrier();
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

CHAOS_IL2CPP_ONCE_FLAG chaos_type_init_once_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c;

void chaos_ensure_type_initialized_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c()
{
	CHAOS_IL2CPP_CALL_ONCE(chaos_type_init_once_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c, []()
	{
	if (chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c____9 == 0)
	{
	    auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c);
	    chaos_object->header.type_info = &chaos_mt_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c.hot;
		const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
		ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c__ctor(chaos_arg_0);
	    chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c____9 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	});
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
extern "C" CHAOS_IL2CPP_INT32 ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_cctor(void);
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[21] = {
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
	1u,
	1u,
	1u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[20];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		20u,
		1u,
		2392377064u,  // FNV-1a over entries+params
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
		{ 1u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_0
		{ 1u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_1
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram__ctor
		{ 0u, 1u },  // ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram_Main_System_String
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_cctor
		{ 0u, 0u },  // ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c__ctor
	},
	{
		2u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[4] = {
		0u,
		1548u,
		0u,
		1544u,
	};

	static constexpr const char* s_type_names[4] = {
		"<Module>",
		"ThreadingMonitorInterlockedSubjects",
		"AutoGeneratedProgram",
		"<>c",
	};

	static constexpr const char* s_type_namespaces[4] = {
		"",
		"",
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[4] = {
		0u,
		0u,
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[4] = {
		nullptr,
		&chaos_mt_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects.hot,
		nullptr,
		&chaos_mt_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c.hot,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[1] = {
		33554436u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[5] = {
		0u,
		0u,
		1u,
		1u,
		1u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[5] = {
		0u,
		0u,
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
		/* .nested_type_children= */ s_nested_type_children,
		/* .nested_type_offset = */ s_nested_type_offset,
		/* .generic_param_constraint_data= */ nullptr,
		/* .generic_param_constraint_offset= */ s_generic_param_constraint_offset,
		/* .type_count        = */ 4u,
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
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[20] = {
	{ ".ctor", 0x00000017u, 0u },  // AutoGeneratedProgram
	{ "Main", 0x00000016u, 1u },  // AutoGeneratedProgram
	{ "CustomEntrySubject_0", 0x00000008u, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_1", 0x00000009u, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "Subject_2", 0x0000000Au, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "Subject_3", 0x0000000Bu, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_4", 0x0000000Cu, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_5", 0x0000000Du, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_6", 0x0000000Eu, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_7", 0x0000000Fu, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_8", 0x00000010u, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_9", 0x00000011u, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_10", 0x00000012u, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "CustomEntrySubject_11", 0x00000013u, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "Subject_12", 0x00000014u, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "Subject_13", 0x00000015u, 0u },  // ThreadingMonitorInterlockedSubjects
	{ "<Subject_13>b__14_0", 0x0000001Au, 0u },  // ThreadingMonitorInterlockedSubjects+<>c
	{ "<Subject_13>b__14_1", 0x0000001Bu, 0u },  // ThreadingMonitorInterlockedSubjects+<>c
	{ ".cctor", 0x00000018u, 0u },  // ThreadingMonitorInterlockedSubjects+<>c
	{ ".ctor", 0x00000019u, 0u },  // ThreadingMonitorInterlockedSubjects+<>c
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[3] = {
	{ "AutoGeneratedProgram", "", 0u, 2u },
	{ "ThreadingMonitorInterlockedSubjects", "", 2u, 14u },
	{ "ThreadingMonitorInterlockedSubjects+<>c", "", 16u, 4u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[20] = {
	{ 0x00000008u, 0u },
	{ 0x00000009u, 1u },
	{ 0x0000000Au, 2u },
	{ 0x0000000Bu, 3u },
	{ 0x0000000Cu, 4u },
	{ 0x0000000Du, 5u },
	{ 0x0000000Eu, 6u },
	{ 0x0000000Fu, 7u },
	{ 0x00000010u, 8u },
	{ 0x00000011u, 9u },
	{ 0x00000012u, 10u },
	{ 0x00000013u, 11u },
	{ 0x00000014u, 12u },
	{ 0x00000015u, 13u },
	{ 0x00000016u, 17u },
	{ 0x00000017u, 16u },
	{ 0x00000018u, 18u },
	{ 0x00000019u, 19u },
	{ 0x0000001Au, 14u },
	{ 0x0000001Bu, 15u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[20] = {
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::Subject_2
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::Subject_3
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_5
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_7
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_8
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_9
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_10
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingMonitorInterlockedSubjects::CustomEntrySubject_11
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::Subject_12
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects::Subject_13
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects+<>c::<Subject_13>b__14_0
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects+<>c::<Subject_13>b__14_1
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGeneratedProgram::.ctor
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram_Main_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGeneratedProgram::Main
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_cctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingMonitorInterlockedSubjects+<>c::.cctor
	{ reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingMonitorInterlockedSubjects+<>c::.ctor
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ThreadingMonitorInterlockedSubjects",
	s_hotpatch_types,
	3u,
	s_hotpatch_methods,
	20u,
	s_hotpatch_slots,
	20u,
	s_hotpatch_entries,
	20u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[12] = {
	"System.Private.CoreLib/System.Int32",
	"System.Threading/Monitor::TryEnter:System.Boolean(System.Object)",
	"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode",
	"System.Threading/Monitor::TryEnter:System.Boolean(System.Object,System.Int32)",
	"System.Threading/Interlocked::MemoryBarrier:System.Void()",
	"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects+<>c::<>9__14_0",
	"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects+<>c::<>9",
	"System.Private.CoreLib/System.Func<System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Private.CoreLib/System.Func<System.Int32>::Invoke:System.Int32()",
	"ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects+<>c::<>9__14_1",
	"System.Threading/Volatile::Read:System.Int32(System.Int32&)",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
};

extern "C" void* kChaosExternalRuntimeFnTable[12] = {
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
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 12;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[20])() = {
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
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_0),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_1),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram__ctor),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram_Main_System_String),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_cctor),
	reinterpret_cast<void(*)()>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c__ctor),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[20])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {kAotMethods[3]();},
	[]() {kAotMethods[4]();},
	[]() {kAotMethods[5]();},
	[]() {kAotMethods[6]();},
	[]() {kAotMethods[7]();},
	[]() {kAotMethods[8]();},
	[]() {kAotMethods[9]();},
	[]() {kAotMethods[10]();},
	[]() {kAotMethods[11]();},
	[]() {kAotMethods[12]();},
	[]() {kAotMethods[13]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[14])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[15])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[16])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[17])(0);},
	[]() {kAotMethods[18]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[19])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 14;
extern "C" const int kSubjectEntryIndices[14] = {
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

	/* ── Entry 0: ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry0;
	/* ── Entry 1: ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry1;
	/* ── Entry 2: ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry2;
	/* ── Entry 3: ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry3;
	/* ── Entry 4: ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry4;
	/* ── Entry 5: ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry5;
	/* ── Entry 6: ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry6;
	/* ── Entry 7: ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram_Main_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry7;
	/* ── Entry 8: ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_cctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry8;
	/* ── Entry 9: ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry9;
} kChaosGcSlotMapsSection = {
	/* entry0 = ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2 */
	.entry0 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_2),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry1 = ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3 */
	.entry1 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_3),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry2 = ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12 */
	.entry2 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry3 = ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13 */
	.entry3 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry4 = ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_0 */
	.entry4 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry5 = ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_1 */
	.entry5 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_1),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry6 = ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram__ctor */
	.entry6 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry7 = ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram_Main_System_String */
	.entry7 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram_Main_System_String),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry8 = ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_cctor */
	.entry8 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_cctor),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry9 = ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c__ctor */
	.entry9 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 316u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[20] = {
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
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_0),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_1),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram__ctor),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram_Main_System_String),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_cctor),
	reinterpret_cast<void*>(&ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c__ctor),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0x82474BDF18568CD0),
		0x00000003u,
		0u,
		3u,
		reinterpret_cast<const void*>(kSlots_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c),
		reinterpret_cast<const void**>(chaos_vtable_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c),
		3u,
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
	.method_pointer_count      = 20u,
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
	.image_name_utf8    = "ThreadingMonitorInterlockedSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects[1] = {
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects[14] = {
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_5:System.Void()", "CustomEntrySubject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_9:System.Void()", "CustomEntrySubject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_10:System.Void()", "CustomEntrySubject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_11:System.Void()", "CustomEntrySubject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_12:System.Void()", "Subject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c[4] = {
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects+<>c::<Subject_13>b__14_0:System.Int32()", "<Subject_13>b__14_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects+<>c::<Subject_13>b__14_1:System.Int32()", "<Subject_13>b__14_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects+<>c::.cctor:System.Void()", ".cctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects+<>c::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram[2] = {
	{ 0u, "ThreadingMonitorInterlockedSubjects/AutoGeneratedProgram::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/AutoGeneratedProgram::Main:System.Void(System.String[])", "Main", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[3] = {
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects", "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects", "", "ThreadingMonitorInterlockedSubjects", "ThreadingMonitorInterlockedSubjects", nullptr, kReflFields_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects, 1u, nullptr, 0u,
	kReflMethods_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects, 14u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects+<>c", "ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects+<>c", "", "ThreadingMonitorInterlockedSubjects+<>c", "ThreadingMonitorInterlockedSubjects+<>c", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c, 4u },
	{ 0u, "ThreadingMonitorInterlockedSubjects/AutoGeneratedProgram", "ThreadingMonitorInterlockedSubjects/AutoGeneratedProgram", "", "AutoGeneratedProgram", "AutoGeneratedProgram", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram, 2u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[3] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ThreadingMonitorInterlockedSubjects", kReflTypePtrs, 3u };

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


	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_1()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


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
	CHAOS_IL2CPP_INTPTR _s17{};
	CHAOS_IL2CPP_INTPTR _s18{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_value = _s0;
				auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_System_Private_CoreLib_System_Int32, {});
				chaos_boxed->header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
				chaos_boxed->value = chaos_value;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
			}
			{
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[1])(chaos_arg_0);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_value = _s3;
				auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_System_Private_CoreLib_System_Int32, {});
				chaos_boxed->header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
				chaos_boxed->value = chaos_value;
				_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
			}
			{
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[1])(chaos_arg_0);
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			chaos_locals[0] = _s4;
			_s4 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s5;
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
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
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
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
	CHAOS_IL2CPP_INTPTR _s19{};
	CHAOS_IL2CPP_INTPTR _s20{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_value = _s0;
				auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_System_Private_CoreLib_System_Int32, {});
				chaos_boxed->header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
				chaos_boxed->value = chaos_value;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
			}
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = ChaosMonitorTryEnter(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_value = _s3;
				auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_System_Private_CoreLib_System_Int32, {});
				chaos_boxed->header.type_info = &chaos_mt_System_Private_CoreLib_System_Int32.hot;
				chaos_boxed->value = chaos_value;
				_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
			}
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				const auto chaos_arg_1 = _s4;
				const auto chaos_arg_0 = _s3;
				const auto chaos_result = ChaosMonitorTryEnter(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
				_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			// brtrue (structured EH branch)
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			// br (handled via structured EH branches)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
			chaos_locals[0] = _s4;
			_s4 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s5;
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
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
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_4()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_5()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_6()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_7()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_8()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_9()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_10()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::CustomEntrySubject_11()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_CustomEntrySubject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_12()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			{
				ChaosInterlockedMemoryBarrier();
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
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects::Subject_13()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects_Subject_13(void)
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
	CHAOS_IL2CPP_INTPTR _s19{};
	CHAOS_IL2CPP_INTPTR _s20{};


	CHAOS_EH_TRY
			chaos_ensure_type_initialized_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c();
			_s0 = chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c____9__14_0;
			_s1 = _s0;
			// brtrue (structured EH branch)
			chaos_ensure_type_initialized_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c();
			_s1 = chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 14)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[14];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[8];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						CHAOS_IL2CPP_INT32 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0);
				};
			_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s2;
				const auto chaos_target = _s1;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s2 = _s1;
			{
				auto chaos_value = _s2;
			chaos_ensure_type_initialized_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c();
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c____9__14_0 = chaos_value;
			}
			{
				const auto chaos_delegate_value = _s1;
				if (chaos_delegate_value == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				auto* chaos_delegate = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Func_System_Int32_*>(chaos_delegate_value);
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
						auto* chaos_invocation_delegate = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Func_System_Int32_*>(chaos_invocation_delegate_value);
						if (chaos_invocation_delegate->chaos_delegate_method_ptr == 0)
						{
							CHAOS_IL2CPP_FAIL();
						}
						if (chaos_invocation_delegate->chaos_delegate_target == 0)
						{
							const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)()>(chaos_invocation_delegate->chaos_delegate_method_ptr);
							chaos_result = chaos_open_function();
						}
						else
						{
							const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
							chaos_result = chaos_closed_function(chaos_invocation_delegate->chaos_delegate_target);
						}
					}
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				else
				{
					if (chaos_delegate->chaos_delegate_method_ptr == 0)
					{
						CHAOS_IL2CPP_FAIL();
					}
					CHAOS_IL2CPP_INT32 __chaos_hotpatch_result{};
					bool __chaos_hotpatch_taken = false;
					uint64_t __chaos_args_buf[1] = {0};
					uint64_t __chaos_ret_buf[2] = {};
					if (chaos_delegate->chaos_delegate_method_token != 0)
					{
						__chaos_hotpatch_taken = ::chaos::il2cpp::runtime_core::DelegateHotpatchCheckpoint(
							chaos_delegate->chaos_delegate_method_token,
							__chaos_args_buf, __chaos_ret_buf, 0);
						if (__chaos_hotpatch_taken)
						{
							__chaos_hotpatch_result = *reinterpret_cast<const CHAOS_IL2CPP_INT32*>(__chaos_ret_buf);
						}
					}

					if (__chaos_hotpatch_taken)
					{
						_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(__chaos_hotpatch_result);
					}
					else
					{
						if (chaos_delegate->chaos_delegate_target == 0)
						{
							const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)()>(chaos_delegate->chaos_delegate_method_ptr);
							const auto chaos_result = chaos_open_function();
							_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
						}
						else
						{
							const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_delegate->chaos_delegate_method_ptr);
							const auto chaos_result = chaos_closed_function(chaos_delegate->chaos_delegate_target);
							_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
						}
					}
				}
			}
			chaos_ensure_type_initialized_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c();
			_s5 = chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c____9__14_1;
			_s6 = _s5;
			// brtrue (structured EH branch)
			chaos_ensure_type_initialized_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c();
			_s6 = chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 15)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[15];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[8];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						CHAOS_IL2CPP_INT32 _d_ret{};
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, &_d_ret);
						return _d_ret;
					}
					return reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0);
				};
			_s7 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s7;
				const auto chaos_target = _s6;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s6 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s7 = _s6;
			{
				auto chaos_value = _s7;
			chaos_ensure_type_initialized_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c();
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c____9__14_1 = chaos_value;
			}
			{
				const auto chaos_delegate_value = _s6;
				if (chaos_delegate_value == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				auto* chaos_delegate = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Func_System_Int32_*>(chaos_delegate_value);
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
						auto* chaos_invocation_delegate = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Func_System_Int32_*>(chaos_invocation_delegate_value);
						if (chaos_invocation_delegate->chaos_delegate_method_ptr == 0)
						{
							CHAOS_IL2CPP_FAIL();
						}
						if (chaos_invocation_delegate->chaos_delegate_target == 0)
						{
							const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)()>(chaos_invocation_delegate->chaos_delegate_method_ptr);
							chaos_result = chaos_open_function();
						}
						else
						{
							const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
							chaos_result = chaos_closed_function(chaos_invocation_delegate->chaos_delegate_target);
						}
					}
					_s6 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				else
				{
					if (chaos_delegate->chaos_delegate_method_ptr == 0)
					{
						CHAOS_IL2CPP_FAIL();
					}
					CHAOS_IL2CPP_INT32 __chaos_hotpatch_result{};
					bool __chaos_hotpatch_taken = false;
					uint64_t __chaos_args_buf[1] = {0};
					uint64_t __chaos_ret_buf[2] = {};
					if (chaos_delegate->chaos_delegate_method_token != 0)
					{
						__chaos_hotpatch_taken = ::chaos::il2cpp::runtime_core::DelegateHotpatchCheckpoint(
							chaos_delegate->chaos_delegate_method_token,
							__chaos_args_buf, __chaos_ret_buf, 0);
						if (__chaos_hotpatch_taken)
						{
							__chaos_hotpatch_result = *reinterpret_cast<const CHAOS_IL2CPP_INT32*>(__chaos_ret_buf);
						}
					}

					if (__chaos_hotpatch_taken)
					{
						_s7 = static_cast<CHAOS_IL2CPP_INTPTR>(__chaos_hotpatch_result);
					}
					else
					{
						if (chaos_delegate->chaos_delegate_target == 0)
						{
							const auto chaos_open_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)()>(chaos_delegate->chaos_delegate_method_ptr);
							const auto chaos_result = chaos_open_function();
							_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
						}
						else
						{
							const auto chaos_closed_function = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_delegate->chaos_delegate_method_ptr);
							const auto chaos_result = chaos_closed_function(chaos_delegate->chaos_delegate_target);
							_s9 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
						}
					}
				}
			}
			_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s8) == static_cast<CHAOS_IL2CPP_INTPTR>(_s9) ? 1 : 0);
			_s9 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s8) == static_cast<CHAOS_IL2CPP_INTPTR>(_s9) ? 1 : 0);
			chaos_locals[0] = _s8;
			_s8 = chaos_locals[0];
			// brfalse (structured EH branch)
			_s9 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s9;
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
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
				chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects+<>c::<Subject_13>b__14_0()
extern "C" CHAOS_IL2CPP_INT32 ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosVolatileRead(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects+<>c::<Subject_13>b__14_1()
extern "C" CHAOS_IL2CPP_INT32 ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_Subject_13_b__14_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_locals[0] = _s0;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosVolatileRead(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ThreadingMonitorInterlockedSubjects/AutoGeneratedProgram::.ctor()
extern "C" void ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: ThreadingMonitorInterlockedSubjects/AutoGeneratedProgram::Main(System.String[])
extern "C" void ThreadingMonitorInterlockedSubjects_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects+<>c::.cctor()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c_cctor(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};

	chaos_ensure_type_initialized_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c();

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c, {});
		chaos_object->header.type_info = &chaos_mt_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c.hot;
		ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
	chaos_ensure_type_initialized_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c();
		chaos_static_ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c____9 = chaos_value;
	}
	return;
}

// Managed method: ThreadingMonitorInterlockedSubjects/ThreadingMonitorInterlockedSubjects+<>c::.ctor()
extern "C" void ThreadingMonitorInterlockedSubjects_ThreadingMonitorInterlockedSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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



}  // namespace chaos::il2cpp::codegen::ThreadingMonitorInterlockedSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 20;

extern "C" void ChaosJitRegisterAll() {}