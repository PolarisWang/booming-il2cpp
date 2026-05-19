#include <chaos/common.h>
#include <chaos/type_info.h>
#include "runtime_core.h"
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
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(12315111816129013648ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);inline TypeInfoV0 chaos_mt_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c, 871985485055265288ULL, 5u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c = static_cast<CHAOS_IL2CPP_INTPTR>(871985485055265288ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Action = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 14722794505377461205ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Action = static_cast<CHAOS_IL2CPP_INTPTR>(14722794505377461205ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_NullReferenceException = {{nullptr, nullptr, 5314911908653589872ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_NullReferenceException = static_cast<CHAOS_IL2CPP_INTPTR>(5314911908653589872ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {{nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {{nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {{nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {{nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {{nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_String = {{nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);
// ── Virtual method table arrays ──
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c[] =
{
	reinterpret_cast<void*>(DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor),
	reinterpret_cast<void*>(DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0),
	reinterpret_cast<void*>(DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0),
	reinterpret_cast<void*>(DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0),
	reinterpret_cast<void*>(DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c[] =
{
	{ 0x00000014u, reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor) },
	{ 0x00000015u, reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0) },
	{ 0x00000016u, reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0) },
	{ 0x00000017u, reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0) },
	{ 0x00000018u, reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0) },
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

struct chaos_type_System_Private_CoreLib_System_NullReferenceException
{
	ThinLockableHeader header{};
	CHAOS_IL2CPP_INTPTR _message = 0;
	CHAOS_IL2CPP_INTPTR _innerException = 0;
	CHAOS_IL2CPP_INTPTR _stackTrace = 0;
	CHAOS_IL2CPP_INT32 _HResult = 0;
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
CHAOS_IL2CPP_INTPTR chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__4_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__5_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__8_0 = 0;
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
// Forward declarations for native function dispatch thunks (defined in delegate_thunks.cpp).
struct RuntimeState;
struct ThreadState;
extern "C" void* NativeDfnThunkArity0(struct RuntimeState*, struct ThreadState*, void*);
extern "C" void* NativeDfnThunkArity1(struct RuntimeState*, struct ThreadState*, void*, void*);
extern "C" void* NativeDfnThunkArity2(struct RuntimeState*, struct ThreadState*, void*, void*, void*);
extern "C" void* NativeDfnThunkArity3(struct RuntimeState*, struct ThreadState*, void*, void*, void*, void*);
extern "C" void* NativeDfnThunkArity4(struct RuntimeState*, struct ThreadState*, void*, void*, void*, void*, void*);

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
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_3(void);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_4(void);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_5(void);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_6(void);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_7(void);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_8(void);
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_9(void);
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
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[17] = {
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
	::ChaosAbiMethodEntryV0 entries[16];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		16u,
		0u,
		187360325u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_0
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_1
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_2
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_3
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_4
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_5
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_6
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_7
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_8
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_9
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_cctor
		{ 0u, 0u },  // DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor
	},
	{
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
		"DelegateCoreInvocationSubjects",
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
		&chaos_mt_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects.hot,
		&chaos_mt_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c.hot,
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
		/* .type_count        = */ 3u,
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
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[16] = {
	{ "Subject_0", 0x00000009u, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_1", 0x0000000Au, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_2", 0x0000000Bu, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_3", 0x0000000Cu, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_4", 0x0000000Du, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_5", 0x0000000Eu, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_6", 0x0000000Fu, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_7", 0x00000010u, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_8", 0x00000011u, 0u },  // DelegateCoreInvocationSubjects
	{ "Subject_9", 0x00000012u, 0u },  // DelegateCoreInvocationSubjects
	{ "<Subject_2>b__3_0", 0x00000015u, 0u },  // DelegateCoreInvocationSubjects+<>c
	{ "<Subject_3>b__4_0", 0x00000016u, 0u },  // DelegateCoreInvocationSubjects+<>c
	{ "<Subject_4>b__5_0", 0x00000017u, 0u },  // DelegateCoreInvocationSubjects+<>c
	{ "<Subject_7>b__8_0", 0x00000018u, 0u },  // DelegateCoreInvocationSubjects+<>c
	{ ".cctor", 0x00000013u, 0u },  // DelegateCoreInvocationSubjects+<>c
	{ ".ctor", 0x00000014u, 0u },  // DelegateCoreInvocationSubjects+<>c
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "DelegateCoreInvocationSubjects", "", 0u, 10u },
	{ "DelegateCoreInvocationSubjects+<>c", "", 10u, 6u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[16] = {
	{ 0x00000009u, 0u },
	{ 0x0000000Au, 1u },
	{ 0x0000000Bu, 2u },
	{ 0x0000000Cu, 4u },
	{ 0x0000000Du, 6u },
	{ 0x0000000Eu, 8u },
	{ 0x0000000Fu, 9u },
	{ 0x00000010u, 10u },
	{ 0x00000011u, 12u },
	{ 0x00000012u, 13u },
	{ 0x00000013u, 14u },
	{ 0x00000014u, 15u },
	{ 0x00000015u, 3u },
	{ 0x00000016u, 5u },
	{ 0x00000017u, 7u },
	{ 0x00000018u, 11u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[16] = {
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // DelegateCoreInvocationSubjects::Subject_0
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // DelegateCoreInvocationSubjects::Subject_1
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // DelegateCoreInvocationSubjects::Subject_2
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects+<>c::<Subject_2>b__3_0
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // DelegateCoreInvocationSubjects::Subject_3
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects+<>c::<Subject_3>b__4_0
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // DelegateCoreInvocationSubjects::Subject_4
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects+<>c::<Subject_4>b__5_0
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects::Subject_5
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects::Subject_6
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // DelegateCoreInvocationSubjects::Subject_7
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects+<>c::<Subject_7>b__8_0
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects::Subject_8
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects::Subject_9
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_cctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // DelegateCoreInvocationSubjects+<>c::.cctor
	{ reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // DelegateCoreInvocationSubjects+<>c::.ctor
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"DelegateCoreInvocationSubjects",
	s_hotpatch_types,
	2u,
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

extern "C" const char* kChaosExternalRuntimeSubjects[16] = {
	"System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)",
	"DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects::_exitCode",
	"System.Private.CoreLib/System.Delegate::Remove:System.Delegate(System.Delegate,System.Delegate)",
	"DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<>9__3_0",
	"DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<>9",
	"System.Private.CoreLib/System.Action::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Private.CoreLib/System.Array::Empty<System.Object>:System.Object[]()",
	"System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])",
	"DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<>9__4_0",
	"System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()",
	"System.Private.CoreLib/System.Object::GetHashCode:System.Int32()",
	"DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<>9__5_0",
	"System.Private.CoreLib/System.Delegate::get_Target:System.Object()",
	"DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<>9__8_0",
	"System.Private.CoreLib/System.Delegate::GetInvocationList:System.Delegate[]()",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
};

extern "C" void* kChaosExternalRuntimeFnTable[16] = {
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Delegate__Combine_System_Delegate_System_Delegate_System_Delegate_),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Delegate__Remove_System_Delegate_System_Delegate_System_Delegate_),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 16;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[16])() = {
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_8),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_cctor),
	reinterpret_cast<void(*)()>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
static void (*kBenchmarkWrappers[16])() = {
	[]() {kAotMethods[0]();},
	[]() {kAotMethods[1]();},
	[]() {kAotMethods[2]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[3])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[4]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[5])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[6]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[7])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[8]();},
	[]() {kAotMethods[9]();},
	[]() {kAotMethods[10]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[11])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[12]();},
	[]() {kAotMethods[13]();},
	[]() {kAotMethods[14]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[15])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
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

// Pure AOT benchmark: calls kAotMethods[i] directly, no hotpatch overhead.
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
// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[16] = {
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_0),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_1),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_2),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_2_b__3_0),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_3),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_4),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_5),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_6),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_7),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_8),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects_Subject_9),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_cctor),
	reinterpret_cast<void*>(&DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c__ctor),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0x0C19EA2DBE040A08),
		0x00000002u,
		0u,
		5u,
		reinterpret_cast<const void*>(kSlots_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c),
		reinterpret_cast<const void**>(chaos_vtable_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c),
		5u,
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
	.vtable_descriptor_count = 1u,
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

static constexpr ReflectionQueryMethodDescriptor kReflMethods_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c[6] = {
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_2>b__3_0:System.Void()", "<Subject_2>b__3_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_3>b__4_0:System.Void()", "<Subject_3>b__4_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_4>b__5_0:System.Void()", "<Subject_4>b__5_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_7>b__8_0:System.Void()", "<Subject_7>b__8_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::.cctor:System.Void()", ".cctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects", "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects", "", "DelegateCoreInvocationSubjects", "DelegateCoreInvocationSubjects", nullptr, kReflFields_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects, 1u, nullptr, 0u,
	kReflMethods_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects, 10u },
	{ 0u, "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c", "DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c", "", "DelegateCoreInvocationSubjects+<>c", "DelegateCoreInvocationSubjects+<>c", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c, 6u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

static const ReflectionQueryImageDescriptor kReflImage = { "DelegateCoreInvocationSubjects", kReflTypePtrs, 2u };

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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			_s0 = 0;
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Delegate__Combine_System_Delegate_System_Delegate_System_Delegate_(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			_s0 = 0;
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Delegate__Combine_System_Delegate_System_Delegate_System_Delegate_(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
			}
	}
		__except(CHAOS_SEH_FILTER_ALL())
	{
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(
			chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			_s0 = 0;
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Delegate__Combine_System_Delegate_System_Delegate_System_Delegate_(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
			}
		}
		else
		{
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(
			chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			_s0 = 0;
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Delegate__Remove_System_Delegate_System_Delegate_System_Delegate_(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				throw;
			}
		}
		_s0 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			_s0 = 0;
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Delegate__Remove_System_Delegate_System_Delegate_System_Delegate_(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
			}
	}
		__except(CHAOS_SEH_FILTER_ALL())
	{
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(
			chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			_s0 = 0;
			_s1 = 0;
			{
				const auto chaos_arg_1 = _s1;
				const auto chaos_arg_0 = _s0;
				const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Delegate__Remove_System_Delegate_System_Delegate_System_Delegate_(chaos_arg_0, chaos_arg_1);
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
			}
		}
		else
		{
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(
			chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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


#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	try
	{
			chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
			_s0 = chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__3_0;
			_s1 = _s0;
			{
				if (_s1 == 0)
				{
					chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
					_s0 = chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9;
					{
						// Hotpatch-aware ldftn wrapper (slot 10)
						static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
							auto& _d_entry = s_hotpatch_entries[10];
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
							{
								alignas(16) uint8_t _d_ab[8];
								ArgBuffer _d_bw(_d_ab);
								_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d_entry.method_key, _d_ab, nullptr);
								return;
							}
							reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0);
						};
					_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
					}
					{
						const auto chaos_method_ptr = _s1;
						const auto chaos_target = _s0;
						auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action, {});
						chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Action.hot;
						chaos_object->chaos_delegate_target = chaos_target;
						chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
						_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
					}
					_s1 = _s0;
					{
						auto chaos_value = _s1;
					chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
						chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__3_0 = chaos_value;
					}
				}
				{
					const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[6])();
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				{
					const auto chaos_arg_0 = _s1;
					const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0);
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s1;
					chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
				}
			}
	}
	catch (const chaos_managed_exception& chaos_exception)
	{
		if (chaos_exception.object_value < 0) { throw; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_exception.object_value);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				throw;
			}
		}
		_s1 = chaos_exception.object_value;
	}
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
	__try
	{
			chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
			_s0 = chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__3_0;
			_s1 = _s0;
			{
				if (_s1 == 0)
				{
					chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
					_s0 = chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9;
					{
						// Hotpatch-aware ldftn wrapper (slot 10)
						static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
							auto& _d_entry = s_hotpatch_entries[10];
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
							{
								alignas(16) uint8_t _d_ab[8];
								ArgBuffer _d_bw(_d_ab);
								_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d_entry.method_key, _d_ab, nullptr);
								return;
							}
							reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0);
						};
					_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
					}
					{
						const auto chaos_method_ptr = _s1;
						const auto chaos_target = _s0;
						auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action, {});
						chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Action.hot;
						chaos_object->chaos_delegate_target = chaos_target;
						chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
						_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
					}
					_s1 = _s0;
					{
						auto chaos_value = _s1;
					chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
						chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__3_0 = chaos_value;
					}
				}
				{
					const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[6])();
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				{
					const auto chaos_arg_0 = _s1;
					const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0);
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s1;
					chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
				}
			}
	}
		__except(CHAOS_SEH_FILTER_ALL())
	{
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(
			chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
	}
#else
	{
		auto* _chaos_jmp =
			chaos::il2cpp::runtime_core::push_exception_jmp_buf();
		if (setjmp(*_chaos_jmp) == 0)
		{
			chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
			_s0 = chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__3_0;
			_s1 = _s0;
			{
				if (_s1 == 0)
				{
					chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
					_s0 = chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9;
					{
						// Hotpatch-aware ldftn wrapper (slot 10)
						static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
							auto& _d_entry = s_hotpatch_entries[10];
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
							{
								alignas(16) uint8_t _d_ab[8];
								ArgBuffer _d_bw(_d_ab);
								_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d_entry.method_key, _d_ab, nullptr);
								return;
							}
							reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0);
						};
					_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
					}
					{
						const auto chaos_method_ptr = _s1;
						const auto chaos_target = _s0;
						auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action, {});
						chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Action.hot;
						chaos_object->chaos_delegate_target = chaos_target;
						chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
						_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
					}
					_s1 = _s0;
					{
						auto chaos_value = _s1;
					chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
						chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__3_0 = chaos_value;
					}
				}
				{
					const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[6])();
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				{
					const auto chaos_arg_0 = _s1;
					const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[7])(chaos_arg_0);
					_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s1;
					chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
				}
			}
		}
		else
		{
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		if (reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj) < 0)
		{
			chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
			chaos::il2cpp::runtime_core::chaos_raise_exception(
				reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
					chaos::il2cpp::runtime_core::g_chaos_exception_obj));
		}
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(
			chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_NullReferenceException.hot))
			{
				chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
				chaos::il2cpp::runtime_core::chaos_raise_exception(
					reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
						chaos::il2cpp::runtime_core::g_chaos_exception_obj));
			}
		}
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos::il2cpp::runtime_core::g_chaos_exception_obj);
		}
		chaos::il2cpp::runtime_core::pop_exception_jmp_buf();
	}
#endif
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


	chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
	_s0 = chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__4_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
			_s0 = chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 11)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[11];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[8];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, nullptr);
						return;
					}
					reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0);
				};
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s1;
				const auto chaos_target = _s0;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Action.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = _s0;
			{
				auto chaos_value = _s1;
			chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
				chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__4_0 = chaos_value;
			}
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[9])();
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_arg_0 = _s1;
			if (chaos_arg_0 == 0)
			{
				::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
			}
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(659009488);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
		{
			if (_s1 != 0)
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s1;
					chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
				}
			}
			return;
		}
	}
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_3>b__4_0()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_3_b__4_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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


	chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
	_s0 = chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__5_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
			_s0 = chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 12)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[12];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[8];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, nullptr);
						return;
					}
					reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0);
				};
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s1;
				const auto chaos_target = _s0;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Action.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = _s0;
			{
				auto chaos_value = _s1;
			chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
				chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__5_0 = chaos_value;
			}
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[12])();
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			const auto chaos_arg_0 = _s1;
			if (chaos_arg_0 == 0)
			{
				::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
			}
			const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Object__GetHashCode_System_Int32__(chaos_arg_0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(35342034);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
		{
			if (_s1 != 0)
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s1;
					chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
				}
			}
			return;
		}
	}
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_4>b__5_0()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_4_b__5_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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


	chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
	_s0 = chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__8_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
			_s0 = chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 13)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[13];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[8];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, nullptr);
						return;
					}
					reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0);
				};
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s1;
				const auto chaos_target = _s0;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Action.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = _s0;
			{
				auto chaos_value = _s1;
			chaos_ensure_type_initialized_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c();
				chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c____9__8_0 = chaos_value;
			}
		}
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[14])();
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1));
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
		{
			if (_s1 != 0)
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s1;
					chaos_static_DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___exitCode = chaos_value;
				}
			}
			return;
		}
	}
}

// Managed method: DelegateCoreInvocationSubjects/DelegateCoreInvocationSubjects+<>c::<Subject_7>b__8_0()
extern "C" void DelegateCoreInvocationSubjects_DelegateCoreInvocationSubjects___c_Subject_7_b__8_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	return;
}



}  // namespace chaos::il2cpp::codegen::DelegateCoreInvocationSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 16;

extern "C" void ChaosJitRegisterAll() {}