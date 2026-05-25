#include <chaos/common.h>
#include <chaos/type_info.h>
#include "runtime_core.h"
#include <chaos/eh.h>
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
#include "enum_stubs.h"
#include "enum_metadata.generated.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

// Forward declarations (pipeline fix: used before extern "C" decl)
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9(void);

namespace chaos::il2cpp::codegen::ThreadingSyncSubjects {

// Bring runtime_core and jit declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::jit;

// Runtime prelude dependencies included at translation unit level


inline MethodTable chaos_mt_System_Private_CoreLib_System_Object = {nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_INTPTR>(5474029880995115448ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_INTPTR>(17082367815459723707ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {nullptr, nullptr, 4137207361503509124ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_INTPTR>(4137207361503509124ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_INTPTR>(17040031516751226236ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {nullptr, nullptr, 10748947813473285525ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_INTPTR>(10748947813473285525ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_String = {nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_INTPTR>(1782325859292956794ULL);
inline MethodTable chaos_mt_System_Threading_AutoResetEvent = {nullptr, nullptr, 9111865296079522101ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Threading_AutoResetEvent = static_cast<CHAOS_IL2CPP_INTPTR>(9111865296079522101ULL);
inline MethodTable chaos_mt_System_Threading_ManualResetEvent = {nullptr, nullptr, 8137036073600629176ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Threading_ManualResetEvent = static_cast<CHAOS_IL2CPP_INTPTR>(8137036073600629176ULL);
inline MethodTable chaos_mt_System_Threading_SemaphoreSlim = {nullptr, nullptr, 10823106288603913950ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Threading_SemaphoreSlim = static_cast<CHAOS_IL2CPP_INTPTR>(10823106288603913950ULL);
inline MethodTable chaos_mt_System_Threading_SpinLock = {nullptr, nullptr, 8630705894240840840ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Threading_SpinLock = static_cast<CHAOS_IL2CPP_INTPTR>(8630705894240840840ULL);
inline MethodTable chaos_mt_ThreadingSyncSubjects_ThreadingSyncSubjects = {nullptr, nullptr, 4025799244050259528ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ThreadingSyncSubjects_ThreadingSyncSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(4025799244050259528ULL);

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
		return chaos_array->element_type_shape == chaos_type_shape_reference            && chaos_is_type_compatible(chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot(), chaos_array->element_type_info);
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

struct chaos_type_System_Threading_AutoResetEvent
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Threading_ManualResetEvent
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Threading_SemaphoreSlim
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Threading_SpinLock
{
	ThinLockableHeader header{};
};

struct chaos_type_ThreadingSyncSubjects_ThreadingSyncSubjects
{
	ThinLockableHeader header{};
};


// ── GcTypeLayout pointer offset arrays (computed via offsetof) ──
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};

extern "C" void ChaosRegisterGcLayouts() {
	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();

	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);
	registry.Register(9111865296079522101ULL, sizeof(chaos_type_System_Threading_AutoResetEvent), nullptr, 0);
	registry.Register(8137036073600629176ULL, sizeof(chaos_type_System_Threading_ManualResetEvent), nullptr, 0);
	registry.Register(10823106288603913950ULL, sizeof(chaos_type_System_Threading_SemaphoreSlim), nullptr, 0);
	registry.Register(8630705894240840840ULL, sizeof(chaos_type_System_Threading_SpinLock), nullptr, 0);
	registry.Register(4025799244050259528ULL, sizeof(chaos_type_ThreadingSyncSubjects_ThreadingSyncSubjects), nullptr, 0);

	// Register MethodTable address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Threading_AutoResetEvent), reinterpret_cast<uintptr_t>(&chaos_mt_System_Threading_AutoResetEvent) + sizeof(chaos_mt_System_Threading_AutoResetEvent));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Threading_ManualResetEvent), reinterpret_cast<uintptr_t>(&chaos_mt_System_Threading_ManualResetEvent) + sizeof(chaos_mt_System_Threading_ManualResetEvent));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Threading_SemaphoreSlim), reinterpret_cast<uintptr_t>(&chaos_mt_System_Threading_SemaphoreSlim) + sizeof(chaos_mt_System_Threading_SemaphoreSlim));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Threading_SpinLock), reinterpret_cast<uintptr_t>(&chaos_mt_System_Threading_SpinLock) + sizeof(chaos_mt_System_Threading_SpinLock));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_ThreadingSyncSubjects_ThreadingSyncSubjects), reinterpret_cast<uintptr_t>(&chaos_mt_ThreadingSyncSubjects_ThreadingSyncSubjects) + sizeof(chaos_mt_ThreadingSyncSubjects_ThreadingSyncSubjects));
}

CHAOS_IL2CPP_INT32 chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects___exitCode = 0;

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };


// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: System.Threading/SemaphoreSlim::.ctor:System.Void(System.Int32)
extern "C" void chaos_bridge_thunk_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[0])(chaos_fn_arg_0);
}

// Bridge/import thunk for: ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode
extern "C" void chaos_bridge_thunk_1(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[1])();
}

// Bridge/import thunk for: System.Threading/SemaphoreSlim::.ctor:System.Void(System.Int32,System.Int32)
extern "C" void chaos_bridge_thunk_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[2])(chaos_fn_arg_0, chaos_fn_arg_1);
}

// Bridge/import thunk for: System.Threading/ManualResetEvent::.ctor:System.Void(System.Boolean)
extern "C" void chaos_bridge_thunk_3(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_fn_arg_0);
}

// Bridge/import thunk for: System.Threading/AutoResetEvent::.ctor:System.Void(System.Boolean)
extern "C" void chaos_bridge_thunk_4(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[4])(chaos_fn_arg_0);
}

// Bridge/import thunk for: System.Threading/SpinLock::.ctor:System.Void(System.Boolean)
extern "C" void chaos_bridge_thunk_5(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[5])(chaos_fn_arg_0);
}

extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15(void);
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[18] = {
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
	::ChaosAbiMethodEntryV0 entries[17];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		17u,
		0u,
		199918157u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15
		{ 0u, 0u },  // ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16
	},
	{
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
		"ThreadingSyncSubjects",
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
		chaos_mt_ThreadingSyncSubjects_ThreadingSyncSubjects.AsTypeInfoHot(),
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
		/* .name_utf8         = */ "ThreadingSyncSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("ThreadingSyncSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[17] = {
	{ "Subject_0", 0x00000003u, 0u },  // ThreadingSyncSubjects
	{ "Subject_1", 0x00000004u, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_2", 0x00000005u, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_3", 0x00000006u, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_4", 0x00000007u, 0u },  // ThreadingSyncSubjects
	{ "Subject_5", 0x00000008u, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_6", 0x00000009u, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_7", 0x0000000Au, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_8", 0x0000000Bu, 0u },  // ThreadingSyncSubjects
	{ "Subject_9", 0x0000000Cu, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_10", 0x0000000Du, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_11", 0x0000000Eu, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_12", 0x0000000Fu, 0u },  // ThreadingSyncSubjects
	{ "Subject_13", 0x00000010u, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_14", 0x00000011u, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_15", 0x00000012u, 0u },  // ThreadingSyncSubjects
	{ "CustomEntrySubject_16", 0x00000013u, 0u },  // ThreadingSyncSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "ThreadingSyncSubjects", "", 0u, 17u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[17] = {
	{ 0x00000003u, 0u },
	{ 0x00000004u, 1u },
	{ 0x00000005u, 2u },
	{ 0x00000006u, 3u },
	{ 0x00000007u, 4u },
	{ 0x00000008u, 5u },
	{ 0x00000009u, 6u },
	{ 0x0000000Au, 7u },
	{ 0x0000000Bu, 8u },
	{ 0x0000000Cu, 9u },
	{ 0x0000000Du, 10u },
	{ 0x0000000Eu, 11u },
	{ 0x0000000Fu, 12u },
	{ 0x00000010u, 13u },
	{ 0x00000011u, 14u },
	{ 0x00000012u, 15u },
	{ 0x00000013u, 16u },
};

// Dispatch table (function pointers)
// s_hotpatch_entries — external linkage for verification_dispatch
// NOTE: `static` is deliberately omitted so that the verification dispatch
// TU (verification_dispatch.generated.cpp) can access this array via
// GetHotpatchEntries(). Without external linkage, the linker cannot resolve
// the symbol across translation units.
extern "C" HotpatchEntryV0 s_hotpatch_entries[17] = {
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingSyncSubjects::Subject_0
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingSyncSubjects::Subject_1
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingSyncSubjects::CustomEntrySubject_2
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingSyncSubjects::CustomEntrySubject_3
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingSyncSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingSyncSubjects::Subject_5
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingSyncSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingSyncSubjects::CustomEntrySubject_7
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingSyncSubjects::CustomEntrySubject_8
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingSyncSubjects::Subject_9
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingSyncSubjects::CustomEntrySubject_10
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingSyncSubjects::CustomEntrySubject_11
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingSyncSubjects::CustomEntrySubject_12
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ThreadingSyncSubjects::Subject_13
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingSyncSubjects::CustomEntrySubject_14
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingSyncSubjects::CustomEntrySubject_15
	{ reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ThreadingSyncSubjects::CustomEntrySubject_16
};

// Accessor functions for verification_dispatch.generated.cpp
extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept { return s_hotpatch_entries; }
extern "C" int32_t GetHotpatchEntryCount() noexcept { return 17; }

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ThreadingSyncSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	17u,
	s_hotpatch_slots,
	17u,
	s_hotpatch_entries,
	17u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[6] = {
	"System.Threading/SemaphoreSlim::.ctor:System.Void(System.Int32)",
	"ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode",
	"System.Threading/SemaphoreSlim::.ctor:System.Void(System.Int32,System.Int32)",
	"System.Threading/ManualResetEvent::.ctor:System.Void(System.Boolean)",
	"System.Threading/AutoResetEvent::.ctor:System.Void(System.Boolean)",
	"System.Threading/SpinLock::.ctor:System.Void(System.Boolean)",
};

extern "C" void* kChaosExternalRuntimeFnTable[6] = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 6;
// (no method AOT entries for this module)
// ── Method table (kMethodTable[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
// NOTE: Pure data only — dispatch routing is in <chaos/hotpatch_dispatch.h>.
// JIT mode uses direct_ptr (precode trampoline → JIT compilation).

static void (*kMethodTable[17])() = {
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15),
	reinterpret_cast<void(*)()>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16),
};

// ── Default-arg thunks (kDefaultArgThunks[]) ────────────────────────────
// Each thunk supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_null_instance = 0;
extern "C" void (*kDefaultArgThunks[17])() = {
	[]() {kMethodTable[0]();},
	[]() {kMethodTable[1]();},
	[]() {kMethodTable[2]();},
	[]() {kMethodTable[3]();},
	[]() {kMethodTable[4]();},
	[]() {kMethodTable[5]();},
	[]() {kMethodTable[6]();},
	[]() {kMethodTable[7]();},
	[]() {kMethodTable[8]();},
	[]() {kMethodTable[9]();},
	[]() {kMethodTable[10]();},
	[]() {kMethodTable[11]();},
	[]() {kMethodTable[12]();},
	[]() {kMethodTable[13]();},
	[]() {kMethodTable[14]();},
	[]() {kMethodTable[15]();},
	[]() {kMethodTable[16]();},
};

// ── Subject slot map (kSubjectSlotMap[]) ────────────────────────────
// Maps subject index (0-based sequential) to kMethodTable index.
// External consumers use this to route --benchmark N to the correct
// AOT method slot, since kMethodTable[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 17;
extern "C" const int kSubjectSlotMap[17] = {
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
	14,
	15,
	16,
};
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

	/* ── Entry 0: ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry0;
	/* ── Entry 1: ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry1;
	/* ── Entry 2: ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry2;
	/* ── Entry 3: ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry3;
	/* ── Entry 4: ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry4;
} kChaosGcSlotMapsSection = {
	/* entry0 = ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry1 = ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1 */
	.entry1 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry2 = ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5 */
	.entry2 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry3 = ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9 */
	.entry3 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry4 = ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13 */
	.entry4 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 124u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[17] = {
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15),
	reinterpret_cast<void*>(&ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 17u,
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
	.image_name_utf8    = "ThreadingSyncSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ThreadingSyncSubjects_ThreadingSyncSubjects[1] = {
	{ 0x04000001u, "ThreadingSyncSubjects/ThreadingSyncSubjects::_exitCode", "_exitCode", "System.Int32", 0LL, 3u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ThreadingSyncSubjects_ThreadingSyncSubjects[17] = {
	{ 0x00000003u, "ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000004u, "ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000005u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_2:System.Void()", "CustomEntrySubject_2", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000006u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_3:System.Void()", "CustomEntrySubject_3", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000007u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000008u, "ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000009u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Au, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Bu, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Cu, "ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_9:System.Void()", "Subject_9", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Du, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_10:System.Void()", "CustomEntrySubject_10", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Eu, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_11:System.Void()", "CustomEntrySubject_11", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Fu, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_12:System.Void()", "CustomEntrySubject_12", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000010u, "ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_13:System.Void()", "Subject_13", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000011u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_14:System.Void()", "CustomEntrySubject_14", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000012u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_15:System.Void()", "CustomEntrySubject_15", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000013u, "ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_16:System.Void()", "CustomEntrySubject_16", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0x00000001u, "ThreadingSyncSubjects/ThreadingSyncSubjects", "ThreadingSyncSubjects/ThreadingSyncSubjects", "", "ThreadingSyncSubjects", "ThreadingSyncSubjects", nullptr, kReflFields_ThreadingSyncSubjects_ThreadingSyncSubjects, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_ThreadingSyncSubjects_ThreadingSyncSubjects, 17u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ThreadingSyncSubjects", kReflTypePtrs, 1u, 1, 0, 0, 0 };

// Fake ImageHandle that ResolveSubjectId will decode back to kReflImage.
// BootstrapRuntime's aot_image_handle fallback discovers this via
// LookupModule(mid)->image at lines 311-321 of bootstrap.cpp.
	// Register generated enum metadata (tables + dispatch + type descriptors).
	// ChaosRegisterEnumGeneratedMetadata is defined in enum_metadata.generated.h.
	static const CHAOS_IL2CPP_UINT32 s_enum_registered = []() noexcept {
		ChaosRegisterEnumGeneratedMetadata();
		return 1u;
	}();

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
// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_0()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_SemaphoreSlim, {});
				chaos_object->header.type_info = chaos_mt_System_Threading_SemaphoreSlim.AsTypeInfoHot();
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_1()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_SemaphoreSlim, {});
				chaos_object->header.type_info = chaos_mt_System_Threading_SemaphoreSlim.AsTypeInfoHot();
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_2()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_3()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_4()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_5()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_ManualResetEvent, {});
				chaos_object->header.type_info = chaos_mt_System_Threading_ManualResetEvent.AsTypeInfoHot();
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_6()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_7()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_8()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_9()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_AutoResetEvent, {});
				chaos_object->header.type_info = chaos_mt_System_Threading_AutoResetEvent.AsTypeInfoHot();
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_10()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_11()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_12()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::Subject_13()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_Subject_13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Threading_SpinLock, {});
				chaos_object->header.type_info = chaos_mt_System_Threading_SpinLock.AsTypeInfoHot();
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			{
				auto chaos_value = _s0;
				chaos_static_ThreadingSyncSubjects_ThreadingSyncSubjects___exitCode = chaos_value;
			}
	CHAOS_EH_END
	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_14()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_14(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_15()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_15(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ThreadingSyncSubjects/ThreadingSyncSubjects::CustomEntrySubject_16()
extern "C" void ThreadingSyncSubjects_ThreadingSyncSubjects_CustomEntrySubject_16(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}



}  // namespace chaos::il2cpp::codegen::ThreadingSyncSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 17;

extern "C" void ChaosJitRegisterAll() {}