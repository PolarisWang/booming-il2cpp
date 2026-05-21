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

namespace chaos::il2cpp::codegen::DelegateCoreInvocationSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c[];
inline TypeInfoV0 chaos_mt_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects = {{nullptr, nullptr, 12315111816129013648ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(12315111816129013648ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c, 871985485055265288ULL, 9u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c = static_cast<CHAOS_IL2CPP_INTPTR>(871985485055265288ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Action = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 14722794505377461205ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Action = static_cast<CHAOS_IL2CPP_INTPTR>(14722794505377461205ULL);
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
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c[] =
{
	reinterpret_cast<void*>(DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor),
	reinterpret_cast<void*>(DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0),
	reinterpret_cast<void*>(DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_1),
	reinterpret_cast<void*>(DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0),
	reinterpret_cast<void*>(DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_1),
	reinterpret_cast<void*>(DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0),
	reinterpret_cast<void*>(DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_1),
	reinterpret_cast<void*>(DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0),
	reinterpret_cast<void*>(DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_1),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c[] =
{
	{ 0x0000001Bu, reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor) },
	{ 0x0000001Cu, reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0) },
	{ 0x0000001Du, reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_1) },
	{ 0x0000001Eu, reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0) },
	{ 0x0000001Fu, reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_1) },
	{ 0x00000020u, reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0) },
	{ 0x00000021u, reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_1) },
	{ 0x00000022u, reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0) },
	{ 0x00000023u, reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_1) },
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

struct chaos_type_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c : public chaos_type_System_Private_CoreLib_System_Object
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

struct chaos_type_System_Private_CoreLib_System_Action : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
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

CHAOS_IL2CPP_INTPTR chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__3_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__3_1 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__4_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__4_1 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__5_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__5_1 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__8_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__8_1 = 0;
CHAOS_IL2CPP_INT32 chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = 0;

using chaos_delegate_invocation_list = CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_INTPTR);

// struct chaos_type_System_Private_CoreLib_System_Delegate is defined by the
// object model (EmitObjectModelDeclarations) with proper : public System_Object
// inheritance. Only the size assertion lives here to avoid C2011 redefinition.
static_assert(sizeof(chaos_type_System_Private_CoreLib_System_Delegate) == 56,
	"System.Delegate struct size must be 56 bytes");

CHAOS_IL2CPP_INTPTR chaos_delegate_allocate_with_type_info(const TypeInfo* chaos_delegate_type_info)
{
	switch (chaos_delegate_type_info->stable_id)
	{
		case chaos_type_id_System_Private_CoreLib_System_Action:
		{
			auto* chaos_delegate = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action, {});
			chaos_delegate->header.type_info = &chaos_mt_System_Private_CoreLib_System_Action.hot;
			return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_delegate);
		}
		default:
			CHAOS_IL2CPP_FAIL();
	}
}

// Forward declarations for native function dispatch thunks (defined in delegate_thunks.cpp).
// These are used by chaos_register_delegate_thunks() when generated delegate thunk entries
// reference &NativeDfnThunkArityN. The definitions live in delegate_thunks.cpp.
// Note: void* parameters instead of typed RuntimeState*/ThreadState* because those types
// are defined in header_layouts.cpp (not a header) and are invisible to the generated code.
extern "C" void* NativeDfnThunkArity0(void*, void*, void*);
extern "C" void* NativeDfnThunkArity1(void*, void*, void*, void*);
extern "C" void* NativeDfnThunkArity2(void*, void*, void*, void*, void*);
extern "C" void* NativeDfnThunkArity3(void*, void*, void*, void*, void*, void*);
extern "C" void* NativeDfnThunkArity4(void*, void*, void*, void*, void*, void*, void*);

// Delegate combine/remove forwarded to runtime-core for unified DelegateObject layout.
// The runtime-core implementations use the invocation_list vector model and allocate
// through old-gen (conservative GC scan), consistent with bridge-created delegates.

CHAOS_IL2CPP_INTPTR chaos_delegate_combine(CHAOS_IL2CPP_INTPTR chaos_left_value, CHAOS_IL2CPP_INTPTR chaos_right_value)
{
	return chaos::il2cpp::runtime_core::DelegateCombine(chaos_left_value, chaos_right_value);
}

CHAOS_IL2CPP_INTPTR chaos_delegate_remove(CHAOS_IL2CPP_INTPTR chaos_source_value, CHAOS_IL2CPP_INTPTR chaos_value_to_remove)
{
	return chaos::il2cpp::runtime_core::DelegateRemove(chaos_source_value, chaos_value_to_remove);
}

CHAOS_IL2CPP_INTPTR chaos_delegate_create_multicast_like(
	CHAOS_IL2CPP_INTPTR chaos_template_delegate_value,
	const chaos_delegate_invocation_list& chaos_entries)
{
	return chaos::il2cpp::runtime_core::DelegateCreateMulticast(chaos_template_delegate_value, chaos_entries);
}
// Auto-generated delegate thunk registrations
// Each delegate type that flows through Marshal.GetDelegateForFunctionPointer /
// Marshal.GetFunctionPointerForDelegate gets a per-signature dispatch thunk
// and a call to RegisterDelegateThunk.

// Delegate type: System.Private.CoreLib/System.Action


// ── Registration calls ──
// These populate the native-side delegate thunk registry so that
// MarshalGetFunctionPointerForDelegate and MarshalGetDelegateForFunctionPointer
// can find the correct dispatch thunk for each delegate type.


static CHAOS_IL2CPP_INTPTR g_thunk_target_chaos_type_System_Private_CoreLib_System_Action = 0;



void chaos_register_delegate_thunks() noexcept {
	RegisterDelegateThunk(
		"System.Private.CoreLib/System.Action",
		reinterpret_cast<void*>(&NativeDfnThunkArity0),
&g_thunk_target_chaos_type_System_Private_CoreLib_System_Action,
0    );

}
static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Delegate__Combine_System_Delegate_System_Delegate_System_Delegate_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	return chaos_delegate_combine(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Delegate__Remove_System_Delegate_System_Delegate_System_Delegate_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	return chaos_delegate_remove(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return ChaosObjectGetHashCode(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

CHAOS_IL2CPP_ONCE_FLAG chaos_type_init_once_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c;

void chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c()
{
	CHAOS_IL2CPP_CALL_ONCE(chaos_type_init_once_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c, []()
	{
	if (chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9 == 0)
	{
	    auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c);
	    chaos_object->header.type_info = &chaos_mt_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c.hot;
		const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
		DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor(chaos_arg_0);
	    chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	});
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_0(void);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_1(void);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_2(void);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_3(void);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_4(void);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_5(void);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_6(void);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_7(void);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_8(void);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_9(void);
extern "C" void DelegateCoreInvocationSubjects_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_cctor(void);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[23] = {
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
	1u,
	1u,
	1u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[22];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		22u,
		1u,
		2129234768u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_0
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_1
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_2
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_1
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_3
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_1
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_4
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_1
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_5
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_6
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_7
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_1
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_8
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_9
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_AutoGeneratedProgram__ctor
		{ 0u, 1u },  // DelegateCoreInvocationSubjects_AutoGeneratedProgram_Main_System_String
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_cctor
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor
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
		"DelegateCoreInvocationSubjects",
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
		&chaos_mt_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects.hot,
		nullptr,
		&chaos_mt_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c.hot,
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
		/* .name_utf8         = */ "DelegateCoreInvocationSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("DelegateCoreInvocationSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[22] = {
	{ ".ctor", 0x00000019u, 0u },  // AutoGeneratedProgram
	{ "Main", 0x00000018u, 1u },  // AutoGeneratedProgram
	{ "Subject_0", 0x0000000Eu, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_1", 0x0000000Fu, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_2", 0x00000010u, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_3", 0x00000011u, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_4", 0x00000012u, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_5", 0x00000013u, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_6", 0x00000014u, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_7", 0x00000015u, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_8", 0x00000016u, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_9", 0x00000017u, 0u },  // DelegateCoreInvocationSubjects
	{ "<Subject_2>b__3_0", 0x0000001Cu, 0u },  // DelegateCoreInvocationSubjects+<>c
	{ "<Subject_2>b__3_1", 0x0000001Du, 0u },  // DelegateCoreInvocationSubjects+<>c
	{ "<Subject_3>b__4_0", 0x0000001Eu, 0u },  // DelegateCoreInvocationSubjects+<>c
	{ "<Subject_3>b__4_1", 0x0000001Fu, 0u },  // DelegateCoreInvocationSubjects+<>c
	{ "<Subject_4>b__5_0", 0x00000020u, 0u },  // DelegateCoreInvocationSubjects+<>c
	{ "<Subject_4>b__5_1", 0x00000021u, 0u },  // DelegateCoreInvocationSubjects+<>c
	{ "<Subject_7>b__8_0", 0x00000022u, 0u },  // DelegateCoreInvocationSubjects+<>c
	{ "<Subject_7>b__8_1", 0x00000023u, 0u },  // DelegateCoreInvocationSubjects+<>c
	{ ".cctor", 0x0000001Au, 0u },  // DelegateCoreInvocationSubjects+<>c
	{ ".ctor", 0x0000001Bu, 0u },  // DelegateCoreInvocationSubjects+<>c
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[3] = {
	{ "AutoGeneratedProgram", "", 0u, 2u },
	{ "DelegateCoreInvocationSubjects", "", 2u, 10u },
	{ "DelegateCoreInvocationSubjects+<>c", "", 12u, 10u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[22] = {
	{ 0x0000000Eu, 0u },
	{ 0x0000000Fu, 1u },
	{ 0x00000010u, 2u },
	{ 0x00000011u, 5u },
	{ 0x00000012u, 8u },
	{ 0x00000013u, 11u },
	{ 0x00000014u, 12u },
	{ 0x00000015u, 13u },
	{ 0x00000016u, 16u },
	{ 0x00000017u, 17u },
	{ 0x00000018u, 19u },
	{ 0x00000019u, 18u },
	{ 0x0000001Au, 20u },
	{ 0x0000001Bu, 21u },
	{ 0x0000001Cu, 3u },
	{ 0x0000001Du, 4u },
	{ 0x0000001Eu, 6u },
	{ 0x0000001Fu, 7u },
	{ 0x00000020u, 9u },
	{ 0x00000021u, 10u },
	{ 0x00000022u, 14u },
	{ 0x00000023u, 15u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[22] = {
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // DelegateCoreInvocationSubjects::Subject_0
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // DelegateCoreInvocationSubjects::Subject_1
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // DelegateCoreInvocationSubjects::Subject_2
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects+<>c::<Subject_2>b__3_0
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects+<>c::<Subject_2>b__3_1
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // DelegateCoreInvocationSubjects::Subject_3
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects+<>c::<Subject_3>b__4_0
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects+<>c::<Subject_3>b__4_1
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // DelegateCoreInvocationSubjects::Subject_4
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects+<>c::<Subject_4>b__5_0
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects+<>c::<Subject_4>b__5_1
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects::Subject_5
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects::Subject_6
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // DelegateCoreInvocationSubjects::Subject_7
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects+<>c::<Subject_7>b__8_0
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects+<>c::<Subject_7>b__8_1
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects::Subject_8
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects::Subject_9
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_AutoGeneratedProgram__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGeneratedProgram::.ctor
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_AutoGeneratedProgram_Main_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGeneratedProgram::Main
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_cctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects+<>c::.cctor
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // DelegateCoreInvocationSubjects+<>c::.ctor
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"DelegateCoreInvocationSubjects",
	s_hotpatch_types,
	3u,
	s_hotpatch_methods,
	22u,
	s_hotpatch_slots,
	22u,
	s_hotpatch_entries,
	22u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[20] = {
	"System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)",
	"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()",
	"DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::_exitCode",
	"System.Private.CoreLib/System.Delegate::Remove:System.Delegate(System.Delegate,System.Delegate)",
	"DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<>9__3_0",
	"DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<>9",
	"System.Private.CoreLib/System.Action::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Private.CoreLib/System.Array::Empty<System.Object>:System.Object[]()",
	"System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])",
	"DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<>9__3_1",
	"DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<>9__4_0",
	"System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()",
	"DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<>9__4_1",
	"DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<>9__5_0",
	"System.Private.CoreLib/System.Delegate::get_Target:System.Object()",
	"DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<>9__5_1",
	"DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<>9__8_0",
	"System.Private.CoreLib/System.Delegate::GetInvocationList:System.Delegate[]()",
	"DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<>9__8_1",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
};

extern "C" void* kChaosExternalRuntimeFnTable[20] = {
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Delegate__Combine_System_Delegate_System_Delegate_System_Delegate_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Delegate__Remove_System_Delegate_System_Delegate_System_Delegate_),
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
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 20;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[22])() = {
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_1),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_1),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_1),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_1),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_AutoGeneratedProgram__ctor),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_AutoGeneratedProgram_Main_System_String),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_cctor),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[22])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[3])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[4])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[5]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[6])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[7])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[8]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[9])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[10])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[11]();},
	[]() {kAotMethods[12]();},
	[]() {kAotMethods[13]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[14])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[15])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[16]();},
	[]() {kAotMethods[17]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[18])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[19])(0);},
	[]() {kAotMethods[20]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[21])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 10;
extern "C" const int kSubjectEntryIndices[10] = {
	0,
	1,
	2,
	5,
	8,
	11,
	12,
	13,
	16,
	17,
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

	/* ── Entry 0: DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry0;
	/* ── Entry 1: DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry1;
	/* ── Entry 2: DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry2;
	/* ── Entry 3: DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry3;
	/* ── Entry 4: DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry4;
	/* ── Entry 5: DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry5;
	/* ── Entry 6: DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry6;
	/* ── Entry 7: DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry7;
	/* ── Entry 8: DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry8;
	/* ── Entry 9: DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry9;
	/* ── Entry 10: DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry10;
	/* ── Entry 11: DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[9];
	} entry11;
	/* ── Entry 12: DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry12;
	/* ── Entry 13: DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry13;
	/* ── Entry 14: DelegateCoreInvocationSubjects_AutoGeneratedProgram__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry14;
	/* ── Entry 15: DelegateCoreInvocationSubjects_AutoGeneratedProgram_Main_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry15;
	/* ── Entry 16: DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_cctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry16;
	/* ── Entry 17: DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry17;
} kChaosGcSlotMapsSection = {
	/* entry0 = DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry1 = DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_1),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry2 = DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_2 */
	.entry2 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_2),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry3 = DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0 */
	.entry3 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry4 = DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_1 */
	.entry4 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_1),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry5 = DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_3 */
	.entry5 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_3),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry6 = DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0 */
	.entry6 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry7 = DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_1 */
	.entry7 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_1),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry8 = DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_4 */
	.entry8 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_4),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry9 = DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0 */
	.entry9 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry10 = DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_1 */
	.entry10 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_1),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry11 = DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_7 */
	.entry11 = {
		/* entry_total_size = 56 */ 56u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_7),
		/* frame_size = 72 */ 72u,
		/* num_gc_slots = 9 */ 9u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u }
	},
	/* entry12 = DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0 */
	.entry12 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry13 = DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_1 */
	.entry13 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_1),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry14 = DelegateCoreInvocationSubjects_AutoGeneratedProgram__ctor */
	.entry14 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_AutoGeneratedProgram__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry15 = DelegateCoreInvocationSubjects_AutoGeneratedProgram_Main_System_String */
	.entry15 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_AutoGeneratedProgram_Main_System_String),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry16 = DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_cctor */
	.entry16 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_cctor),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry17 = DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor */
	.entry17 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 544u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[22] = {
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_0),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_1),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_2),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_1),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_3),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_1),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_4),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_1),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_5),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_6),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_7),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_1),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_8),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_9),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_AutoGeneratedProgram__ctor),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_AutoGeneratedProgram_Main_System_String),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_cctor),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0x0C19EA2DBE040A08),
		0x00000003u,
		0u,
		9u,
		reinterpret_cast<const void*>(kSlots_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c),
		reinterpret_cast<const void**>(chaos_vtable_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c),
		9u,
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
	.method_pointer_count      = 22u,
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
	.image_name_utf8    = "DelegateCoreInvocationSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects[1] = {
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects[10] = {
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_8:System.Void()", "Subject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c[10] = {
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_2>b__3_0:System.Void()", "<Subject_2>b__3_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_2>b__3_1:System.Void()", "<Subject_2>b__3_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_3>b__4_0:System.Void()", "<Subject_3>b__4_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_3>b__4_1:System.Void()", "<Subject_3>b__4_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_4>b__5_0:System.Void()", "<Subject_4>b__5_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_4>b__5_1:System.Void()", "<Subject_4>b__5_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_7>b__8_0:System.Void()", "<Subject_7>b__8_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_7>b__8_1:System.Void()", "<Subject_7>b__8_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::.cctor:System.Void()", ".cctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_DelegateCoreInvocationSubjects_AutoGeneratedProgram[2] = {
	{ 0u, "DelegateCoreInvocationSubjects/AutoGeneratedProgram::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/AutoGeneratedProgram::Main:System.Void(System.String[])", "Main", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[3] = {
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects", "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects", "", "DelegateCoreInvocationSubjects", "DelegateCoreInvocationSubjects", nullptr, kReflFields_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects, 1u, nullptr, 0u,
	kReflMethods_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects, 10u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c", "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c", "", "DelegateCoreInvocationSubjects+<>c", "DelegateCoreInvocationSubjects+<>c", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c, 10u },
	{ 0u, "DelegateCoreInvocationSubjects/AutoGeneratedProgram", "DelegateCoreInvocationSubjects/AutoGeneratedProgram", "", "AutoGeneratedProgram", "AutoGeneratedProgram", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_DelegateCoreInvocationSubjects_AutoGeneratedProgram, 2u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[3] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
};

static const ReflectionQueryImageDescriptor kReflImage = { "DelegateCoreInvocationSubjects", kReflTypePtrs, 3u };

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
// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_0()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_0(void)
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
				chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_1()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_1(void)
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
				chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_2()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_2(void)
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
	CHAOS_IL2CPP_INTPTR _s21{};
	CHAOS_IL2CPP_INTPTR _s22{};
	CHAOS_IL2CPP_INTPTR _s23{};
	CHAOS_IL2CPP_INTPTR _s24{};


	CHAOS_EH_TRY
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
				chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_2>b__3_0()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_2>b__3_1()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_3()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_3(void)
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
	CHAOS_IL2CPP_INTPTR _s21{};
	CHAOS_IL2CPP_INTPTR _s22{};


	CHAOS_EH_TRY
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
				chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_3>b__4_0()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_3>b__4_1()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_4()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_4(void)
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
	CHAOS_IL2CPP_INTPTR _s21{};
	CHAOS_IL2CPP_INTPTR _s22{};


	CHAOS_EH_TRY
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
				chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_4>b__5_0()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_4>b__5_1()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_5()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_6()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_7()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_7(void)
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
	CHAOS_IL2CPP_INTPTR _s21{};
	CHAOS_IL2CPP_INTPTR _s22{};
	CHAOS_IL2CPP_INTPTR _s23{};
	CHAOS_IL2CPP_INTPTR _s24{};
	CHAOS_IL2CPP_INTPTR _s25{};
	CHAOS_IL2CPP_INTPTR _s26{};
	CHAOS_IL2CPP_INTPTR _s27{};
	CHAOS_IL2CPP_INTPTR _s28{};


	CHAOS_EH_TRY
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
				chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_7>b__8_0()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_7>b__8_1()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_8()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::Subject_9()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: DelegateCoreInvocationSubjects/AutoGeneratedProgram::.ctor()
extern "C" void DelegateCoreInvocationSubjects_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: DelegateCoreInvocationSubjects/AutoGeneratedProgram::Main(System.String[])
extern "C" void DelegateCoreInvocationSubjects_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::.cctor()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_cctor(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};

	chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c, {});
		chaos_object->header.type_info = &chaos_mt_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c.hot;
		DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
	chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
		chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9 = chaos_value;
	}
	return;
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::.ctor()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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



}  // namespace chaos::il2cpp::codegen::DelegateCoreInvocationSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 22;

extern "C" void ChaosJitRegisterAll() {}