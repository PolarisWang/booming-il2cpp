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

namespace chaos::il2cpp::codegen::ThreadingTasksPrimitivesSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c[];
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Action = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 14722794505377461205ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Action = static_cast<CHAOS_IL2CPP_INTPTR>(14722794505377461205ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Action_System_Threading_Tasks_Task_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 3665167686475733763ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Action_System_Threading_Tasks_Task_ = static_cast<CHAOS_IL2CPP_INTPTR>(3665167686475733763ULL);
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
inline TypeInfoV0 chaos_mt_System_Threading_Thread_System_Threading_Thread = {{nullptr, nullptr, 11502368789179341480ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Threading_Thread_System_Threading_Thread = static_cast<CHAOS_IL2CPP_INTPTR>(11502368789179341480ULL);
inline TypeInfoV0 chaos_mt_System_Threading_Thread_System_Threading_ThreadStart = {{nullptr, nullptr, 17186843898991579444ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Threading_Thread_System_Threading_ThreadStart = static_cast<CHAOS_IL2CPP_INTPTR>(17186843898991579444ULL);
inline TypeInfoV0 chaos_mt_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry = {{nullptr, nullptr, 2186445116007965406ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry = static_cast<CHAOS_IL2CPP_INTPTR>(2186445116007965406ULL);
inline TypeInfoV0 chaos_mt_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c, 16753052439711259458ULL, 7u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c = static_cast<CHAOS_IL2CPP_INTPTR>(16753052439711259458ULL);

// ── Virtual method table arrays ──
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod0_b__2_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod11_b__13_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod1_b__3_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod3_b__5_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_1_System_Threading_Tasks_Task(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
const void* chaos_vtable_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c[] =
{
	reinterpret_cast<void*>(ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c__ctor),
	reinterpret_cast<void*>(ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod0_b__2_0),
	reinterpret_cast<void*>(ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod11_b__13_0),
	reinterpret_cast<void*>(ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod1_b__3_0),
	reinterpret_cast<void*>(ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod3_b__5_0),
	reinterpret_cast<void*>(ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_0),
	reinterpret_cast<void*>(ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_1_System_Threading_Tasks_Task),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c[] =
{
	{ 0x00000020u, reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c__ctor) },
	{ 0x00000021u, reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod0_b__2_0) },
	{ 0x00000026u, reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod11_b__13_0) },
	{ 0x00000022u, reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod1_b__3_0) },
	{ 0x00000023u, reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod3_b__5_0) },
	{ 0x00000024u, reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_0) },
	{ 0x00000025u, reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_1_System_Threading_Tasks_Task) },
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

struct chaos_type_System_Private_CoreLib_System_Action : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_Action_System_Threading_Tasks_Task_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
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

struct chaos_type_System_Threading_Thread_System_Threading_Thread
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Threading_Thread_System_Threading_ThreadStart
{
	ThinLockableHeader header{};
};

struct chaos_type_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry
{
	ThinLockableHeader header{};
};

struct chaos_type_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c : public chaos_type_System_Private_CoreLib_System_Object
{
};

CHAOS_IL2CPP_INTPTR chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__13_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__2_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__3_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__5_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__9_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__9_1 = 0;
CHAOS_IL2CPP_INT32 chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___exitCode = 0;
CHAOS_IL2CPP_INT32 chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry__s_sharedState = 0;

static void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Join_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	chaos_thread_join(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	chaos_thread_sleep(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Start_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	chaos_thread_start(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread___ctor_System_Void_System_Threading_ThreadStart_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	chaos_thread_ctor(chaos_fn_arg_0, chaos_fn_arg_1);
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__(void)
{
	return chaos_thread_get_current();
}

CHAOS_IL2CPP_ONCE_FLAG chaos_type_init_once_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c;

void chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c()
{
	CHAOS_IL2CPP_CALL_ONCE(chaos_type_init_once_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c, []()
	{
	if (chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9 == 0)
	{
	    auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c);
	    chaos_object->header.type_info = &chaos_mt_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c.hot;
		const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
		ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c__ctor(chaos_arg_0);
	    chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	});
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod0(void);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod1(void);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod10(void);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod11(void);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod12(void);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod14(void);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod2(void);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod3(void);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod4(void);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod5(void);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod6(void);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod7(void);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod8(void);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod9(void);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_cctor(void);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod0_b__2_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod1_b__3_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod11_b__13_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod3_b__5_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_1_System_Threading_Tasks_Task(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[25] = {
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
	2u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[24];
	CHAOS_IL2CPP_UINT8 params[2];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		24u,
		2u,
		820179780u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram__ctor
		{ 0u, 1u },  // ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram_Main_System_String
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod0
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod1
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod10
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod11
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod12
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod14
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod2
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod3
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod4
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod5
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod6
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod7
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod8
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod9
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_cctor
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c__ctor
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod0_b__2_0
		{ 1u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod1_b__3_0
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod11_b__13_0
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod3_b__5_0
		{ 0u, 0u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_0
		{ 0u, 1u },  // ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_1_System_Threading_Tasks_Task
	},
	{
		2u,
		2u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[5] = {
		0u,
		1548u,
		1548u,
		0u,
		1544u,
	};

	static constexpr const char* s_type_names[5] = {
		"<Module>",
		"ThreadingTasksPrimitivesSubjects",
		"ThreadingTasksPrimitivesNativeEntry",
		"AutoGeneratedProgram",
		"<>c",
	};

	static constexpr const char* s_type_namespaces[5] = {
		"",
		"",
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
		&chaos_mt_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry.hot,
		nullptr,
		&chaos_mt_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c.hot,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[1] = {
		33554437u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[6] = {
		0u,
		0u,
		0u,
		1u,
		1u,
		1u,
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
		/* .name_utf8         = */ "ThreadingTasksPrimitivesSubjects",
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
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("ThreadingTasksPrimitivesSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[24] = {
	{ ".ctor", 0x0000001Eu, 0u },  // AutoGeneratedProgram
	{ "Main", 0x0000001Du, 1u },  // AutoGeneratedProgram
	{ "CustomEntryMethod0", 0x0000000Fu, 0u },  // ThreadingTasksPrimitivesNativeEntry
	{ "CustomEntryMethod1", 0x00000010u, 0u },  // ThreadingTasksPrimitivesNativeEntry
	{ "CustomEntryMethod10", 0x00000019u, 0u },  // ThreadingTasksPrimitivesNativeEntry
	{ "CustomEntryMethod11", 0x0000001Au, 0u },  // ThreadingTasksPrimitivesNativeEntry
	{ "CustomEntryMethod12", 0x0000001Bu, 0u },  // ThreadingTasksPrimitivesNativeEntry
	{ "CustomEntryMethod14", 0x0000001Cu, 0u },  // ThreadingTasksPrimitivesNativeEntry
	{ "CustomEntryMethod2", 0x00000011u, 0u },  // ThreadingTasksPrimitivesNativeEntry
	{ "CustomEntryMethod3", 0x00000012u, 0u },  // ThreadingTasksPrimitivesNativeEntry
	{ "CustomEntryMethod4", 0x00000013u, 0u },  // ThreadingTasksPrimitivesNativeEntry
	{ "CustomEntryMethod5", 0x00000014u, 0u },  // ThreadingTasksPrimitivesNativeEntry
	{ "CustomEntryMethod6", 0x00000015u, 0u },  // ThreadingTasksPrimitivesNativeEntry
	{ "CustomEntryMethod7", 0x00000016u, 0u },  // ThreadingTasksPrimitivesNativeEntry
	{ "CustomEntryMethod8", 0x00000017u, 0u },  // ThreadingTasksPrimitivesNativeEntry
	{ "CustomEntryMethod9", 0x00000018u, 0u },  // ThreadingTasksPrimitivesNativeEntry
	{ ".cctor", 0x0000001Fu, 0u },  // ThreadingTasksPrimitivesNativeEntry+<>c
	{ ".ctor", 0x00000020u, 0u },  // ThreadingTasksPrimitivesNativeEntry+<>c
	{ "<CustomEntryMethod0>b__2_0", 0x00000021u, 0u },  // ThreadingTasksPrimitivesNativeEntry+<>c
	{ "<CustomEntryMethod1>b__3_0", 0x00000022u, 0u },  // ThreadingTasksPrimitivesNativeEntry+<>c
	{ "<CustomEntryMethod11>b__13_0", 0x00000026u, 0u },  // ThreadingTasksPrimitivesNativeEntry+<>c
	{ "<CustomEntryMethod3>b__5_0", 0x00000023u, 0u },  // ThreadingTasksPrimitivesNativeEntry+<>c
	{ "<CustomEntryMethod7>b__9_0", 0x00000024u, 0u },  // ThreadingTasksPrimitivesNativeEntry+<>c
	{ "<CustomEntryMethod7>b__9_1", 0x00000025u, 1u },  // ThreadingTasksPrimitivesNativeEntry+<>c
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[3] = {
	{ "AutoGeneratedProgram", "", 0u, 2u },
	{ "ThreadingTasksPrimitivesNativeEntry", "", 2u, 14u },
	{ "ThreadingTasksPrimitivesNativeEntry+<>c", "", 16u, 8u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[24] = {
	{ 0x0000000Fu, 2u },
	{ 0x00000010u, 3u },
	{ 0x00000011u, 8u },
	{ 0x00000012u, 9u },
	{ 0x00000013u, 10u },
	{ 0x00000014u, 11u },
	{ 0x00000015u, 12u },
	{ 0x00000016u, 13u },
	{ 0x00000017u, 14u },
	{ 0x00000018u, 15u },
	{ 0x00000019u, 4u },
	{ 0x0000001Au, 5u },
	{ 0x0000001Bu, 6u },
	{ 0x0000001Cu, 7u },
	{ 0x0000001Du, 1u },
	{ 0x0000001Eu, 0u },
	{ 0x0000001Fu, 16u },
	{ 0x00000020u, 17u },
	{ 0x00000021u, 18u },
	{ 0x00000022u, 19u },
	{ 0x00000023u, 21u },
	{ 0x00000024u, 22u },
	{ 0x00000025u, 23u },
	{ 0x00000026u, 20u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[24] = {
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGeneratedProgram::.ctor
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram_Main_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGeneratedProgram::Main
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod0
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod1
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod10
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod11
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod12
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod14
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod2
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod3
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod4
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod5
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod6
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod7
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod8
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod9
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_cctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingTasksPrimitivesNativeEntry+<>c::.cctor
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingTasksPrimitivesNativeEntry+<>c::.ctor
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod0_b__2_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod0>b__2_0
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod1_b__3_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod1>b__3_0
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod11_b__13_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod11>b__13_0
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod3_b__5_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod3>b__5_0
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod7>b__9_0
	{ reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_1_System_Threading_Tasks_Task), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod7>b__9_1
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ThreadingTasksPrimitivesSubjects",
	s_hotpatch_types,
	3u,
	s_hotpatch_methods,
	24u,
	s_hotpatch_slots,
	24u,
	s_hotpatch_entries,
	24u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[32] = {
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
	"ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<>9__2_0",
	"ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<>9",
	"System.Private.CoreLib/System.Action::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task(System.Action)",
	"System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Void()",
	"ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<>9__3_0",
	"System.Private.CoreLib/System.Func<System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Private.CoreLib/System.Threading.Tasks.Task::Run<System.Int32>:System.Threading.Tasks.Task<System.Int32>(System.Func<System.Int32>)",
	"System.Private.CoreLib/System.Threading.Tasks.Task::FromResult<System.Int32>:System.Threading.Tasks.Task<System.Int32>(System.Int32)",
	"System.Private.CoreLib/System.Threading.Tasks.Task<System.Int32>::get_Result:System.Int32()",
	"ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::_exitCode",
	"ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::s_sharedState",
	"ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<>9__13_0",
	"System.Threading.Thread/System.Threading.ThreadStart::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Threading.Thread/System.Threading.Thread::.ctor:System.Void(System.Threading.ThreadStart)",
	"System.Threading.Thread/System.Threading.Thread::Start:System.Void()",
	"System.Threading.Thread/System.Threading.Thread::Join:System.Void()",
	"System.Threading.Thread/System.Threading.Thread::Sleep:System.Void(System.Int32)",
	"System.Threading.Thread/System.Threading.Thread::get_CurrentThread:System.Threading.Thread()",
	"System.Threading.Thread/System.Threading.Thread::get_ManagedThreadId:System.Int32()",
	"System.Private.CoreLib/System.Threading.Tasks.Task::Delay:System.Threading.Tasks.Task(System.Int32)",
	"ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<>9__5_0",
	"System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Boolean(System.Int32)",
	"System.Private.CoreLib/System.Threading.Tasks.Task::get_IsCompleted:System.Boolean()",
	"System.Private.CoreLib/System.Threading.Tasks.Task::get_Status:System.Threading.Tasks.TaskStatus()",
	"ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<>9__9_0",
	"ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<>9__9_1",
	"System.Private.CoreLib/System.Action<System.Threading.Tasks.Task>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Private.CoreLib/System.Threading.Tasks.Task::ContinueWith:System.Threading.Tasks.Task(System.Action<System.Threading.Tasks.Task>)",
	"System.Private.CoreLib/System.Threading.Tasks.Task::WhenAny:System.Threading.Tasks.Task<System.Threading.Tasks.Task>(System.Threading.Tasks.Task,System.Threading.Tasks.Task)",
	"System.Private.CoreLib/System.Threading.Tasks.Task<System.Threading.Tasks.Task>::get_Result:System.Threading.Tasks.Task()",
};

extern "C" void* kChaosExternalRuntimeFnTable[32] = {
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
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread___ctor_System_Void_System_Threading_ThreadStart_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Start_System_Void__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Join_System_Void__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__),
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

extern "C" int32_t kChaosExternalRuntimeCount = 32;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[24])() = {
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram__ctor),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram_Main_System_String),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod0),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod1),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod10),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod11),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod12),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod14),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod2),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod3),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod4),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod5),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod6),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod7),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod8),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod9),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_cctor),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c__ctor),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod0_b__2_0),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod1_b__3_0),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod11_b__13_0),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod3_b__5_0),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_0),
	reinterpret_cast<void(*)()>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_1_System_Threading_Tasks_Task),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[24])() = {
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[0])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[1])(0);},
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
	[]() {kAotMethods[14]();},
	[]() {kAotMethods[15]();},
	[]() {kAotMethods[16]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[17])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[18])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[19])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[20])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[21])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[22])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[23])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
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

	/* ── Entry 0: ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry0;
	/* ── Entry 1: ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram_Main_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry1;
	/* ── Entry 2: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry2;
	/* ── Entry 3: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry3;
	/* ── Entry 4: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod10 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry4;
	/* ── Entry 5: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod11 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry5;
	/* ── Entry 6: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod12 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry6;
	/* ── Entry 7: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod14 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry7;
	/* ── Entry 8: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry8;
	/* ── Entry 9: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry9;
	/* ── Entry 10: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry10;
	/* ── Entry 11: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry11;
	/* ── Entry 12: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry12;
	/* ── Entry 13: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry13;
	/* ── Entry 14: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry14;
	/* ── Entry 15: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_cctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry15;
	/* ── Entry 16: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry16;
	/* ── Entry 17: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod0_b__2_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry17;
	/* ── Entry 18: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod1_b__3_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry18;
	/* ── Entry 19: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod11_b__13_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry19;
	/* ── Entry 20: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod3_b__5_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry20;
	/* ── Entry 21: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry21;
	/* ── Entry 22: ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_1_System_Threading_Tasks_Task ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry22;
} kChaosGcSlotMapsSection = {
	/* entry0 = ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram__ctor */
	.entry0 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry1 = ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram_Main_System_String */
	.entry1 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram_Main_System_String),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry2 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod0 */
	.entry2 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry3 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod1 */
	.entry3 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry4 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod10 */
	.entry4 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod10),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry5 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod11 */
	.entry5 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod11),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry6 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod12 */
	.entry6 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod12),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry7 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod14 */
	.entry7 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod14),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry8 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod2 */
	.entry8 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod2),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry9 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod3 */
	.entry9 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod3),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry10 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod4 */
	.entry10 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod4),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry11 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod5 */
	.entry11 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod5),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry12 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod6 */
	.entry12 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod6),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry13 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod7 */
	.entry13 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod7),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry14 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod9 */
	.entry14 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod9),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry15 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_cctor */
	.entry15 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_cctor),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry16 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c__ctor */
	.entry16 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry17 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod0_b__2_0 */
	.entry17 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod0_b__2_0),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry18 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod1_b__3_0 */
	.entry18 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod1_b__3_0),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry19 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod11_b__13_0 */
	.entry19 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod11_b__13_0),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry20 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod3_b__5_0 */
	.entry20 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod3_b__5_0),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry21 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_0 */
	.entry21 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_0),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry22 = ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_1_System_Threading_Tasks_Task */
	.entry22 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_1_System_Threading_Tasks_Task),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 732u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[24] = {
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram__ctor),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram_Main_System_String),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod0),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod1),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod10),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod11),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod12),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod14),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod2),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod3),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod4),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod5),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod6),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod7),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod8),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod9),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_cctor),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c__ctor),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod0_b__2_0),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod1_b__3_0),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod11_b__13_0),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod3_b__5_0),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_0),
	reinterpret_cast<void*>(&ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_1_System_Threading_Tasks_Task),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0xE87ECC6EB47B1B42),
		0x00000003u,
		0u,
		7u,
		reinterpret_cast<const void*>(kSlots_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c),
		reinterpret_cast<const void**>(chaos_vtable_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c),
		7u,
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
	.method_pointer_count      = 24u,
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
	.image_name_utf8    = "ThreadingTasksPrimitivesSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryMethodDescriptor kReflMethods_ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram[2] = {
	{ 0u, "ThreadingTasksPrimitivesSubjects/AutoGeneratedProgram::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/AutoGeneratedProgram::Main:System.Void(System.String[])", "Main", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryFieldDescriptor kReflFields_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry[2] = {
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::_exitCode", "_exitCode", "System.Int32", 0LL },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::s_sharedState", "s_sharedState", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry[14] = {
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod0:System.Void()", "CustomEntryMethod0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod1:System.Void()", "CustomEntryMethod1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod10:System.Void()", "CustomEntryMethod10", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod11:System.Void()", "CustomEntryMethod11", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod12:System.Void()", "CustomEntryMethod12", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod14:System.Void()", "CustomEntryMethod14", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod2:System.Void()", "CustomEntryMethod2", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod3:System.Void()", "CustomEntryMethod3", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod4:System.Void()", "CustomEntryMethod4", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod5:System.Void()", "CustomEntryMethod5", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod6:System.Void()", "CustomEntryMethod6", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod7:System.Void()", "CustomEntryMethod7", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod8:System.Void()", "CustomEntryMethod8", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod9:System.Void()", "CustomEntryMethod9", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c[8] = {
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::.cctor:System.Void()", ".cctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod0>b__2_0:System.Void()", "<CustomEntryMethod0>b__2_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod1>b__3_0:System.Int32()", "<CustomEntryMethod1>b__3_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod11>b__13_0:System.Void()", "<CustomEntryMethod11>b__13_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod3>b__5_0:System.Void()", "<CustomEntryMethod3>b__5_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod7>b__9_0:System.Void()", "<CustomEntryMethod7>b__9_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod7>b__9_1:System.Void(System.Threading.Tasks.Task)", "<CustomEntryMethod7>b__9_1", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[3] = {
	{ 0u, "ThreadingTasksPrimitivesSubjects/AutoGeneratedProgram", "ThreadingTasksPrimitivesSubjects/AutoGeneratedProgram", "", "AutoGeneratedProgram", "AutoGeneratedProgram", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram, 2u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry", "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry", "", "ThreadingTasksPrimitivesNativeEntry", "ThreadingTasksPrimitivesNativeEntry", nullptr, kReflFields_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry, 2u, nullptr, 0u,
	kReflMethods_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry, 14u },
	{ 0u, "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c", "ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c", "", "ThreadingTasksPrimitivesNativeEntry+<>c", "ThreadingTasksPrimitivesNativeEntry+<>c", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c, 8u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[3] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ThreadingTasksPrimitivesSubjects", kReflTypePtrs, 3u };

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
// Managed method: ThreadingTasksPrimitivesSubjects/AutoGeneratedProgram::.ctor()
extern "C" void ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: ThreadingTasksPrimitivesSubjects/AutoGeneratedProgram::Main(System.String[])
extern "C" void ThreadingTasksPrimitivesSubjects_AutoGeneratedProgram_Main_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod0()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod0(void)
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


	chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
	_s0 = chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__2_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
			_s0 = chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 18)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[18];
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
			chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
				chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__2_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_0 = _s0;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[4])(chaos_arg_0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[0] = _s0;
		_s0 = chaos_locals[0];
		{
			reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[5])();
		}
		return;
	}
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod1()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod1(void)
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


	chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
	_s0 = chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__3_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
			_s0 = chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 19)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> CHAOS_IL2CPP_INT32 {
					auto& _d_entry = s_hotpatch_entries[19];
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
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s1;
				const auto chaos_target = _s0;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Func_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Func_System_Int32_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = _s0;
			{
				auto chaos_value = _s1;
			chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
				chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__3_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_0 = _s0;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[8])(chaos_arg_0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[0] = _s0;
		_s0 = chaos_locals[0];
		{
			reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[5])();
		}
		return;
	}
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod10()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod10(void)
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[10])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
	{
		if (_s1 != 0)
		{
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod11()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod11(void)
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
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};
	CHAOS_IL2CPP_INTPTR _s16{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		auto chaos_value = _s0;
		chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry__s_sharedState = chaos_value;
	}
	chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
	_s0 = chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__13_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
			_s0 = chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 20)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[20];
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
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_Thread_System_Threading_ThreadStart, {});
				chaos_object->header.type_info = &chaos_mt_System_Threading_Thread_System_Threading_ThreadStart.hot;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = _s0;
			{
				auto chaos_value = _s1;
			chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
				chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__13_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_1 = _s0;
			auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_Thread_System_Threading_Thread, {});
			chaos_object->header.type_info = &chaos_mt_System_Threading_Thread_System_Threading_Thread.hot;
			chaos_external_runtime_System_Threading_Thread_System_Threading_Thread___ctor_System_Void_System_Threading_ThreadStart_(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), chaos_arg_1);
			_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
		}
		chaos_locals[0] = _s0;
		_s0 = chaos_locals[0];
		{
			const auto chaos_arg_0 = _s0;
			chaos_thread_start(chaos_arg_0);
		}
		_s0 = chaos_locals[0];
		{
			const auto chaos_arg_0 = _s0;
			chaos_thread_join(chaos_arg_0);
		}
		_s0 = chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry__s_sharedState;
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
		{
			if (_s0 != 0)
			{
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s0;
					chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___exitCode = chaos_value;
				}
			}
			return;
		}
	}
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod12()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		chaos_thread_sleep(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
	}
	return;
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod14()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod14(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};
	CHAOS_IL2CPP_INTPTR _s3{};
	CHAOS_IL2CPP_INTPTR _s4{};
	CHAOS_IL2CPP_INTPTR _s5{};


	{
		const auto chaos_result = chaos_thread_get_current();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[20])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s1) < static_cast<CHAOS_IL2CPP_INT32>(_s2) ? 1 : 0);
	{
		if (_s1 != 0)
		{
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod2()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[21])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[5])();
	}
	return;
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod3()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod3(void)
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


	chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
	_s0 = chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__5_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
			_s0 = chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 21)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[21];
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
			chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
				chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__5_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_0 = _s0;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[4])(chaos_arg_0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[0] = _s0;
		_s0 = chaos_locals[0];
		{
			reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[5])();
		}
		return;
	}
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod4()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod4(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[21])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(5000);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[23])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
	{
		if (_s1 != 0)
		{
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod5()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod5(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[24])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
	{
		if (_s1 != 0)
		{
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod6()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod6(void)
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
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[25])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(5);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
	{
		if (_s1 != 0)
		{
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s1;
				chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod7()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod7(void)
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
	CHAOS_IL2CPP_INTPTR _s10{};
	CHAOS_IL2CPP_INTPTR _s11{};
	CHAOS_IL2CPP_INTPTR _s12{};
	CHAOS_IL2CPP_INTPTR _s13{};
	CHAOS_IL2CPP_INTPTR _s14{};
	CHAOS_IL2CPP_INTPTR _s15{};


	chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
	_s0 = chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__9_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
			_s0 = chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 22)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[22];
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
			chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
				chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__9_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_0 = _s0;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[4])(chaos_arg_0);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
		_s1 = chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__9_1;
		_s2 = _s1;
		{
			if (_s2 == 0)
			{
				chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
				_s1 = chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9;
				{
					// Hotpatch-aware ldftn wrapper (slot 23)
					static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) -> void {
						auto& _d_entry = s_hotpatch_entries[23];
						if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
							&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
						{
							alignas(16) uint8_t _d_ab[16];
							ArgBuffer _d_bw(_d_ab);
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
							_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_1));
							::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
								_d_entry.method_key, _d_ab, nullptr);
							return;
						}
						reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
					};
				_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
				}
				{
					const auto chaos_method_ptr = _s2;
					const auto chaos_target = _s1;
					auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action_System_Threading_Tasks_Task_, {});
					chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Action_System_Threading_Tasks_Task_.hot;
					chaos_object->chaos_delegate_target = chaos_target;
					chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
					_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
				}
				_s2 = _s1;
				{
					auto chaos_value = _s2;
				chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
					chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9__9_1 = chaos_value;
				}
			}
			{
				const auto chaos_arg_0 = _s2;
				const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[29])(chaos_arg_0);
				_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			chaos_locals[1] = _s2;
			_s2 = chaos_locals[1];
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[5])();
			}
			return;
		}
	}
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod8()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry::CustomEntryMethod9()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry_CustomEntryMethod9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 4) chaos_locals{};
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(10000);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[21])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[9])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[1];
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[30])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[2] = _s0;
	_s0 = chaos_locals[2];
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[5])();
	}
	_s1 = chaos_locals[2];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[31])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s3 = chaos_locals[1];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
	{
		if (_s2 != 0)
		{
			_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s2;
				chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___exitCode = chaos_value;
			}
		}
		return;
	}
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::.cctor()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_cctor(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};

	chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c, {});
		chaos_object->header.type_info = &chaos_mt_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c.hot;
		ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
	chaos_ensure_type_initialized_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c();
		chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c____9 = chaos_value;
	}
	return;
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::.ctor()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod0>b__2_0()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod0_b__2_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod1>b__3_0()
extern "C" CHAOS_IL2CPP_INT32 ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod1_b__3_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod11>b__13_0()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod11_b__13_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		auto chaos_value = _s0;
		chaos_static_ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry__s_sharedState = chaos_value;
	}
	return;
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod3>b__5_0()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod3_b__5_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod7>b__9_0()
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: ThreadingTasksPrimitivesSubjects/ThreadingTasksPrimitivesNativeEntry+<>c::<CustomEntryMethod7>b__9_1(System.Threading.Tasks.Task)
extern "C" void ThreadingTasksPrimitivesSubjects_ThreadingTasksPrimitivesNativeEntry___c_CustomEntryMethod7_b__9_1_System_Threading_Tasks_Task(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	return;
}



}  // namespace chaos::il2cpp::codegen::ThreadingTasksPrimitivesSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 24;

extern "C" void ChaosJitRegisterAll() {}