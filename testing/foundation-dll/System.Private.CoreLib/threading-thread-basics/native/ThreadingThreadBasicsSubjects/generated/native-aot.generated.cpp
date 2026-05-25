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
#include "enum_stubs.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod8(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_0(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_1(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_10(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_2(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_3(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_4(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_5(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_6(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_7(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_8(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_9(void);

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod8(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_0(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_1(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_10(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_2(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_3(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_4(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_5(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_6(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_7(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_8(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_9(void);

namespace chaos::il2cpp::codegen::ThreadingThreadBasicsSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException = {{nullptr, nullptr, 3627294964229478006ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_PlatformNotSupportedException = static_cast<CHAOS_IL2CPP_INTPTR>(3627294964229478006ULL);
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
inline TypeInfoV0 chaos_mt_System_Threading_Thread_System_Threading_ThreadInterruptedException = {{nullptr, nullptr, 1689251155357643907ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Threading_Thread_System_Threading_ThreadInterruptedException = static_cast<CHAOS_IL2CPP_INTPTR>(1689251155357643907ULL);

// ── Virtual method table arrays ──
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
		return chaos_array->element_type_shape == chaos_type_shape_reference            && chaos_is_type_compatible(reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_String), chaos_array->element_type_info);
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

struct chaos_type_System_Private_CoreLib_System_PlatformNotSupportedException
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

struct chaos_type_System_Threading_Thread_System_Threading_ThreadInterruptedException
{
	ThinLockableHeader header{};
};


// ── GcTypeLayout pointer offset arrays (computed via offsetof) ──
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_PlatformNotSupportedException[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_PlatformNotSupportedException, _message)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_PlatformNotSupportedException, _innerException)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_PlatformNotSupportedException, _stackTrace))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};

extern "C" void ChaosRegisterGcLayouts() {
	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();

	registry.Register(3627294964229478006ULL, sizeof(chaos_type_System_Private_CoreLib_System_PlatformNotSupportedException), kGcOffsets_chaos_type_System_Private_CoreLib_System_PlatformNotSupportedException, 3);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);
	registry.Register(1689251155357643907ULL, sizeof(chaos_type_System_Threading_Thread_System_Threading_ThreadInterruptedException), nullptr, 0);

	// Register TypeInfoV0 address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException) + sizeof(chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Threading_Thread_System_Threading_ThreadInterruptedException), reinterpret_cast<uintptr_t>(&chaos_mt_System_Threading_Thread_System_Threading_ThreadInterruptedException) + sizeof(chaos_mt_System_Threading_Thread_System_Threading_ThreadInterruptedException));
}

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Interrupt_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	chaos_thread_interrupt(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	chaos_thread_sleep(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Yield_System_Boolean__(void)
{
	return chaos_thread_yield();
}

static CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__(void)
{
	return chaos_thread_get_current();
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_IsBackground_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_thread_is_background(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_IsThreadPoolThread_System_Boolean__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_thread_is_threadpool(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_Priority_System_Threading_ThreadPriority__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_thread_get_priority(chaos_fn_arg_0);
}

static CHAOS_IL2CPP_INT32 chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_ThreadState_System_Threading_ThreadState__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_thread_get_state(chaos_fn_arg_0);
}

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__set_IsBackground_System_Void_System_Boolean_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	chaos_thread_set_background(chaos_fn_arg_0, chaos_fn_arg_1);
}

static void chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__set_Priority_System_Void_System_Threading_ThreadPriority_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	chaos_thread_set_priority(chaos_fn_arg_0, chaos_fn_arg_1);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };


// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: System.Threading.Thread/System.Threading.Thread::Abort:System.Void()
extern "C" void chaos_bridge_thunk_0(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[8])();
}

// Bridge/import thunk for: System.Threading.Thread/System.Threading.Thread::ResetAbort:System.Void()
extern "C" void chaos_bridge_thunk_1(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[10])();
}

extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_0(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_1(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_2(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_3(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_4(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_5(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_6(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_7(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_8(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_9(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_10(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod8(void);
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9(void);


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
	0u,
	0u,
	0u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[22];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		22u,
		0u,
		2614092085u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_0
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_1
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_2
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_3
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_4
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_5
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_6
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_7
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_8
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_9
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_10
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod8
		{ 0u, 0u },  // ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9
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
		1548u,
	};

	static constexpr const char* s_type_names[3] = {
		"<Module>",
		"ThreadingThreadBasicsSubjects",
		"ThreadingThreadBasicsNativeEntry",
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
		nullptr,
		nullptr,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[4] = {
		0u,
		0u,
		0u,
		0u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[4] = {
		0u,
		0u,
		0u,
		0u,
	};



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "ThreadingThreadBasicsSubjects",
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
		/* .type_count        = */ 3u,
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
		::chaos::il2cpp::runtime_core::RegisterModule("ThreadingThreadBasicsSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[22] = {
	{ "CustomEntryMethod0", 0x0000000Fu, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod1", 0x00000010u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod10", 0x00000019u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod2", 0x00000011u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod3", 0x00000012u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod4", 0x00000013u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod5", 0x00000014u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod6", 0x00000015u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod7", 0x00000016u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod8", 0x00000017u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntryMethod9", 0x00000018u, 0u },  // ThreadingThreadBasicsNativeEntry
	{ "CustomEntrySubject_0", 0x00000004u, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_1", 0x00000005u, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_2", 0x00000006u, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_3", 0x00000007u, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_4", 0x00000008u, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_5", 0x00000009u, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_6", 0x0000000Au, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_7", 0x0000000Bu, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_8", 0x0000000Cu, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_9", 0x0000000Du, 0u },  // ThreadingThreadBasicsSubjects
	{ "CustomEntrySubject_10", 0x0000000Eu, 0u },  // ThreadingThreadBasicsSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "ThreadingThreadBasicsNativeEntry", "", 0u, 11u },
	{ "ThreadingThreadBasicsSubjects", "", 11u, 11u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[22] = {
	{ 0x00000004u, 0u },
	{ 0x00000005u, 1u },
	{ 0x00000006u, 2u },
	{ 0x00000007u, 3u },
	{ 0x00000008u, 4u },
	{ 0x00000009u, 5u },
	{ 0x0000000Au, 6u },
	{ 0x0000000Bu, 7u },
	{ 0x0000000Cu, 8u },
	{ 0x0000000Du, 9u },
	{ 0x0000000Eu, 10u },
	{ 0x0000000Fu, 11u },
	{ 0x00000010u, 12u },
	{ 0x00000011u, 14u },
	{ 0x00000012u, 15u },
	{ 0x00000013u, 16u },
	{ 0x00000014u, 17u },
	{ 0x00000015u, 18u },
	{ 0x00000016u, 19u },
	{ 0x00000017u, 20u },
	{ 0x00000018u, 21u },
	{ 0x00000019u, 13u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[22] = {
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_2
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_3
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_5
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_7
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_8
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_9
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingThreadBasicsSubjects::CustomEntrySubject_10
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod0
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod1
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod10
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod2
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod3
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod4
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod5
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod6
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod7
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod8
	{ reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingThreadBasicsNativeEntry::CustomEntryMethod9
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ThreadingThreadBasicsSubjects",
	s_hotpatch_types,
	2u,
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

extern "C" const char* kChaosExternalRuntimeSubjects[12] = {
	"System.Threading.Thread/System.Threading.Thread::get_CurrentThread:System.Threading.Thread()",
	"System.Threading.Thread/System.Threading.Thread::get_ThreadState:System.Threading.ThreadState()",
	"System.Threading.Thread/System.Threading.Thread::get_Priority:System.Threading.ThreadPriority()",
	"System.Threading.Thread/System.Threading.Thread::Sleep:System.Void(System.Int32)",
	"System.Threading.Thread/System.Threading.Thread::set_Priority:System.Void(System.Threading.ThreadPriority)",
	"System.Threading.Thread/System.Threading.Thread::get_IsBackground:System.Boolean()",
	"System.Threading.Thread/System.Threading.Thread::set_IsBackground:System.Void(System.Boolean)",
	"System.Threading.Thread/System.Threading.Thread::get_IsThreadPoolThread:System.Boolean()",
	"System.Threading.Thread/System.Threading.Thread::Abort:System.Void()",
	"System.Threading.Thread/System.Threading.Thread::Interrupt:System.Void()",
	"System.Threading.Thread/System.Threading.Thread::ResetAbort:System.Void()",
	"System.Threading.Thread/System.Threading.Thread::Yield:System.Boolean()",
};

extern "C" void* kChaosExternalRuntimeFnTable[12] = {
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_CurrentThread_System_Threading_Thread__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_ThreadState_System_Threading_ThreadState__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_Priority_System_Threading_ThreadPriority__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Sleep_System_Void_System_Int32_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__set_Priority_System_Void_System_Threading_ThreadPriority_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_IsBackground_System_Boolean__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__set_IsBackground_System_Void_System_Boolean_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__get_IsThreadPoolThread_System_Boolean__),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Interrupt_System_Void__),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Threading_Thread_System_Threading_Thread__Yield_System_Boolean__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 12;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[22])() = {
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_0),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_1),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_2),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_3),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_4),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_5),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_6),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_7),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_8),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_9),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_10),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod8),
	reinterpret_cast<void(*)()>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9),
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
	[]() {kAotMethods[17]();},
	[]() {kAotMethods[18]();},
	[]() {kAotMethods[19]();},
	[]() {kAotMethods[20]();},
	[]() {kAotMethods[21]();},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 11;
extern "C" const int kSubjectEntryIndices[11] = {
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

	/* ── Entry 0: ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry0;
	/* ── Entry 1: ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry1;
	/* ── Entry 2: ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry2;
	/* ── Entry 3: ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry3;
	/* ── Entry 4: ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry4;
	/* ── Entry 5: ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry5;
	/* ── Entry 6: ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry6;
	/* ── Entry 7: ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry7;
	/* ── Entry 8: ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry8;
	/* ── Entry 9: ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry9;
} kChaosGcSlotMapsSection = {
	/* entry0 = ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0 */
	.entry0 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry1 = ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1 */
	.entry1 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry2 = ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10 */
	.entry2 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry3 = ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2 */
	.entry3 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry4 = ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3 */
	.entry4 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry5 = ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4 */
	.entry5 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry6 = ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5 */
	.entry6 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry7 = ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6 */
	.entry7 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry8 = ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7 */
	.entry8 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry9 = ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9 */
	.entry9 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 264u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[22] = {
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_0),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_1),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_2),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_3),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_4),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_5),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_6),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_7),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_8),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_9),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_10),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod8),
	reinterpret_cast<void*>(&ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
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
	.vtable_descriptors = nullptr,
	.vtable_descriptor_count = 0u,
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
	.image_name_utf8    = "ThreadingThreadBasicsSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects[1] = {
	{ 0x04000001u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::_exitCode", "_exitCode", "System.Int32", 0LL, 3u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects[11] = {
	{ 0x00000004u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000005u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000006u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_2:System.Void()", "CustomEntrySubject_2", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000007u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_3:System.Void()", "CustomEntrySubject_3", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000008u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000009u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_5:System.Void()", "CustomEntrySubject_5", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Au, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Bu, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Cu, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Du, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_9:System.Void()", "CustomEntrySubject_9", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Eu, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_10:System.Void()", "CustomEntrySubject_10", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry[11] = {
	{ 0x0000000Fu, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod0:System.Void()", "CustomEntryMethod0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000010u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod1:System.Void()", "CustomEntryMethod1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000019u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod10:System.Void()", "CustomEntryMethod10", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000011u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod2:System.Void()", "CustomEntryMethod2", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000012u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod3:System.Void()", "CustomEntryMethod3", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000013u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod4:System.Void()", "CustomEntryMethod4", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000014u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod5:System.Void()", "CustomEntryMethod5", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000015u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod6:System.Void()", "CustomEntryMethod6", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000016u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod7:System.Void()", "CustomEntryMethod7", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000017u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod8:System.Void()", "CustomEntryMethod8", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000018u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod9:System.Void()", "CustomEntryMethod9", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0x00000001u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects", "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects", "", "ThreadingThreadBasicsSubjects", "ThreadingThreadBasicsSubjects", nullptr, kReflFields_ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects, 11u, nullptr, 0u, 0u },
	{ 0x00000002u, "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry", "ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry", "", "ThreadingThreadBasicsNativeEntry", "ThreadingThreadBasicsNativeEntry", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry, 11u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ThreadingThreadBasicsSubjects", kReflTypePtrs, 2u, 1, 0, 0, 0 };

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
// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_0()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_1()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_2()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_3()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_4()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_5()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_6()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_7()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_8()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_9()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsSubjects::CustomEntrySubject_10()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsSubjects_CustomEntrySubject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod0()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_thread_get_current();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_thread_get_state(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod1()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_thread_get_current();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_thread_get_priority(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod10()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod10(void)
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

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod2()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_thread_get_current();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	{
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_arg_0 = _s0;
		chaos_thread_set_priority(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
	}
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod3()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_thread_get_current();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_thread_is_background(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod4()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_thread_get_current();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_arg_0 = _s0;
		chaos_thread_set_background(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
	}
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod5()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	{
		const auto chaos_result = chaos_thread_get_current();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = chaos_thread_is_threadpool(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod6()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			{
				const auto chaos_result = chaos_thread_get_current();
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
			}
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[8])();
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException)))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
	CHAOS_EH_END
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod7()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	{
		const auto chaos_result = chaos_thread_get_current();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_0 = _s0;
		chaos_thread_interrupt(chaos_arg_0);
	}
	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			{
				const auto chaos_raw_arg_0 = _s0;
				const auto chaos_arg_0 = chaos_raw_arg_0;
				chaos_thread_sleep(static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_0));
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Threading_Thread_System_Threading_ThreadInterruptedException)))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
	CHAOS_EH_END
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod8()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	CHAOS_EH_TRY
			{
				reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[10])();
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), reinterpret_cast<const TypeInfoHot*>(&chaos_mt_System_Private_CoreLib_System_PlatformNotSupportedException)))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
	CHAOS_EH_END
	return;
}

// Managed method: ThreadingThreadBasicsSubjects/ThreadingThreadBasicsNativeEntry::CustomEntryMethod9()
extern "C" void ThreadingThreadBasicsSubjects_ThreadingThreadBasicsNativeEntry_CustomEntryMethod9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};


	{
		const auto chaos_result = chaos_thread_yield();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}



}  // namespace chaos::il2cpp::codegen::ThreadingThreadBasicsSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 22;

extern "C" void ChaosJitRegisterAll() {}