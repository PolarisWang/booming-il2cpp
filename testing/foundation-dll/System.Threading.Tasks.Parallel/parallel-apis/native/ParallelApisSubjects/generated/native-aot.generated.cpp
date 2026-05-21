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

namespace chaos::il2cpp::codegen::ParallelApisSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


const void* chaos_vtable_ParallelApisSubjects_ParallelApisSubjects___c[];
inline TypeInfoV0 chaos_mt_ParallelApisSubjects_ParallelApisSubjects = {{nullptr, nullptr, 8214961744706585336ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ParallelApisSubjects_ParallelApisSubjects = static_cast<CHAOS_IL2CPP_INTPTR>(8214961744706585336ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Object = {{nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_INTPTR>(15228727185366376748ULL);
inline TypeInfoV0 chaos_mt_ParallelApisSubjects_ParallelApisSubjects___c = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, chaos_vtable_ParallelApisSubjects_ParallelApisSubjects___c, 16889560106937221856ULL, 8u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_ParallelApisSubjects_ParallelApisSubjects___c = static_cast<CHAOS_IL2CPP_INTPTR>(16889560106937221856ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Delegate = {{&chaos_mt_System_Private_CoreLib_System_Object.hot, nullptr, 7451128447593600616ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_INTPTR>(7451128447593600616ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {{&chaos_mt_System_Private_CoreLib_System_Delegate.hot, nullptr, 6681393039041505440ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_INTPTR>(6681393039041505440ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Action = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 14722794505377461205ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Action = static_cast<CHAOS_IL2CPP_INTPTR>(14722794505377461205ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_Action_System_Int32_ = {{&chaos_mt_System_Private_CoreLib_System_MulticastDelegate.hot, nullptr, 13738459579961990418ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Action_System_Int32_ = static_cast<CHAOS_IL2CPP_INTPTR>(13738459579961990418ULL);
inline TypeInfoV0 chaos_mt_System_Private_CoreLib_System_ArgumentNullException = {{nullptr, nullptr, 12400915656331929738ULL, 0u, 32, 1, 1}, {nullptr, nullptr, 0, 0, 0, 0}};
inline constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_ArgumentNullException = static_cast<CHAOS_IL2CPP_INTPTR>(12400915656331929738ULL);
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
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_0_b__1_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_1_b__2_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_2_b__3_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_3_b__4_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_41_b__42_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_4_b__5_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_5_b__6_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
const void* chaos_vtable_ParallelApisSubjects_ParallelApisSubjects___c[] =
{
	reinterpret_cast<void*>(ParallelApisSubjects_ParallelApisSubjects___c__ctor),
	reinterpret_cast<void*>(ParallelApisSubjects_ParallelApisSubjects___c_Subject_0_b__1_0_System_Int32),
	reinterpret_cast<void*>(ParallelApisSubjects_ParallelApisSubjects___c_Subject_1_b__2_0_System_Int32),
	reinterpret_cast<void*>(ParallelApisSubjects_ParallelApisSubjects___c_Subject_2_b__3_0_System_Int32),
	reinterpret_cast<void*>(ParallelApisSubjects_ParallelApisSubjects___c_Subject_3_b__4_0_System_Int32),
	reinterpret_cast<void*>(ParallelApisSubjects_ParallelApisSubjects___c_Subject_41_b__42_0),
	reinterpret_cast<void*>(ParallelApisSubjects_ParallelApisSubjects___c_Subject_4_b__5_0_System_Int32),
	reinterpret_cast<void*>(ParallelApisSubjects_ParallelApisSubjects___c_Subject_5_b__6_0_System_Int32),
};
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_ParallelApisSubjects_ParallelApisSubjects___c[] =
{
	{ 0x00000038u, reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c__ctor) },
	{ 0x00000039u, reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_0_b__1_0_System_Int32) },
	{ 0x0000003Au, reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_1_b__2_0_System_Int32) },
	{ 0x0000003Bu, reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_2_b__3_0_System_Int32) },
	{ 0x0000003Cu, reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_3_b__4_0_System_Int32) },
	{ 0x0000003Fu, reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_41_b__42_0) },
	{ 0x0000003Du, reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_4_b__5_0_System_Int32) },
	{ 0x0000003Eu, reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_5_b__6_0_System_Int32) },
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

struct chaos_type_ParallelApisSubjects_ParallelApisSubjects
{
	ThinLockableHeader header{};
};

struct chaos_type_System_Private_CoreLib_System_Object
{
	ThinLockableHeader header{};
};

struct chaos_type_ParallelApisSubjects_ParallelApisSubjects___c : public chaos_type_System_Private_CoreLib_System_Object
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

struct chaos_type_System_Private_CoreLib_System_Action_System_Int32_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate
{
};

struct chaos_type_System_Private_CoreLib_System_ArgumentNullException
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

CHAOS_IL2CPP_INTPTR chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__1_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__2_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__3_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__42_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__4_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__5_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__6_0 = 0;
CHAOS_IL2CPP_INT32 chaos_static_ParallelApisSubjects_ParallelApisSubjects___exitCode = 0;

static void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

CHAOS_IL2CPP_ONCE_FLAG chaos_type_init_once_ParallelApisSubjects_ParallelApisSubjects___c;

void chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c()
{
	CHAOS_IL2CPP_CALL_ONCE(chaos_type_init_once_ParallelApisSubjects_ParallelApisSubjects___c, []()
	{
	if (chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9 == 0)
	{
	    auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_ParallelApisSubjects_ParallelApisSubjects___c);
	    chaos_object->header.type_info = &chaos_mt_ParallelApisSubjects_ParallelApisSubjects___c.hot;
		const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
		ParallelApisSubjects_ParallelApisSubjects___c__ctor(chaos_arg_0);
	    chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	});
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_0(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_0_b__1_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_1(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_1_b__2_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_2(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_2_b__3_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_3(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_3_b__4_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_4(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_4_b__5_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_5(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_5_b__6_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_6(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_7(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_8(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_9(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_10(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_11(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_12(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_13(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_14(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_15(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_16(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_17(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_18(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_19(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_20(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_21(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_22(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_23(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_24(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_25(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_26(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_27(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_28(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_29(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_30(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_31(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_32(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_33(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_34(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_35(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_36(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_37(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_38(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_39(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_40(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_41(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_41_b__42_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_42(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_cctor(void);
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[53] = {
	0u,
	0u,
	1u,
	1u,
	2u,
	2u,
	3u,
	3u,
	4u,
	4u,
	5u,
	5u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
	6u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[52];
	CHAOS_IL2CPP_UINT8 params[6];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		52u,
		6u,
		3777727707u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_Subject_0
		{ 0u, 1u },  // ParallelApisSubjects_ParallelApisSubjects___c_Subject_0_b__1_0_System_Int32
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_Subject_1
		{ 0u, 1u },  // ParallelApisSubjects_ParallelApisSubjects___c_Subject_1_b__2_0_System_Int32
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_Subject_2
		{ 0u, 1u },  // ParallelApisSubjects_ParallelApisSubjects___c_Subject_2_b__3_0_System_Int32
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_Subject_3
		{ 0u, 1u },  // ParallelApisSubjects_ParallelApisSubjects___c_Subject_3_b__4_0_System_Int32
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_Subject_4
		{ 0u, 1u },  // ParallelApisSubjects_ParallelApisSubjects___c_Subject_4_b__5_0_System_Int32
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_Subject_5
		{ 0u, 1u },  // ParallelApisSubjects_ParallelApisSubjects___c_Subject_5_b__6_0_System_Int32
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_Subject_6
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_Subject_7
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_8
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_9
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_10
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_11
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_12
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_13
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_14
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_15
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_16
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_17
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_18
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_19
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_20
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_21
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_22
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_23
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_24
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_25
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_26
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_27
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_28
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_29
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_30
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_31
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_32
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_33
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_34
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_35
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_36
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_37
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_38
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_39
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_40
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_Subject_41
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects___c_Subject_41_b__42_0
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects_Subject_42
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects___c_cctor
		{ 0u, 0u },  // ParallelApisSubjects_ParallelApisSubjects___c__ctor
	},
	{
		1u,
		1u,
		1u,
		1u,
		1u,
		1u,
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
		"ParallelApisSubjects",
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
		&chaos_mt_ParallelApisSubjects_ParallelApisSubjects.hot,
		&chaos_mt_ParallelApisSubjects_ParallelApisSubjects___c.hot,
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
		/* .name_utf8         = */ "ParallelApisSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("ParallelApisSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[52] = {
	{ "Subject_0", 0x0000000Cu, 0u },  // ParallelApisSubjects
	{ "Subject_1", 0x0000000Du, 0u },  // ParallelApisSubjects
	{ "Subject_2", 0x0000000Eu, 0u },  // ParallelApisSubjects
	{ "Subject_3", 0x0000000Fu, 0u },  // ParallelApisSubjects
	{ "Subject_4", 0x00000010u, 0u },  // ParallelApisSubjects
	{ "Subject_5", 0x00000011u, 0u },  // ParallelApisSubjects
	{ "Subject_6", 0x00000012u, 0u },  // ParallelApisSubjects
	{ "Subject_7", 0x00000013u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_8", 0x00000014u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_9", 0x00000015u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_10", 0x00000016u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_11", 0x00000017u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_12", 0x00000018u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_13", 0x00000019u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_14", 0x0000001Au, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_15", 0x0000001Bu, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_16", 0x0000001Cu, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_17", 0x0000001Du, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_18", 0x0000001Eu, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_19", 0x0000001Fu, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_20", 0x00000020u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_21", 0x00000021u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_22", 0x00000022u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_23", 0x00000023u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_24", 0x00000024u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_25", 0x00000025u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_26", 0x00000026u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_27", 0x00000027u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_28", 0x00000028u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_29", 0x00000029u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_30", 0x0000002Au, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_31", 0x0000002Bu, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_32", 0x0000002Cu, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_33", 0x0000002Du, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_34", 0x0000002Eu, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_35", 0x0000002Fu, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_36", 0x00000030u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_37", 0x00000031u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_38", 0x00000032u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_39", 0x00000033u, 0u },  // ParallelApisSubjects
	{ "CustomEntrySubject_40", 0x00000034u, 0u },  // ParallelApisSubjects
	{ "Subject_41", 0x00000035u, 0u },  // ParallelApisSubjects
	{ "Subject_42", 0x00000036u, 0u },  // ParallelApisSubjects
	{ "<Subject_0>b__1_0", 0x00000039u, 1u },  // ParallelApisSubjects+<>c
	{ "<Subject_1>b__2_0", 0x0000003Au, 1u },  // ParallelApisSubjects+<>c
	{ "<Subject_2>b__3_0", 0x0000003Bu, 1u },  // ParallelApisSubjects+<>c
	{ "<Subject_3>b__4_0", 0x0000003Cu, 1u },  // ParallelApisSubjects+<>c
	{ "<Subject_4>b__5_0", 0x0000003Du, 1u },  // ParallelApisSubjects+<>c
	{ "<Subject_5>b__6_0", 0x0000003Eu, 1u },  // ParallelApisSubjects+<>c
	{ "<Subject_41>b__42_0", 0x0000003Fu, 0u },  // ParallelApisSubjects+<>c
	{ ".cctor", 0x00000037u, 0u },  // ParallelApisSubjects+<>c
	{ ".ctor", 0x00000038u, 0u },  // ParallelApisSubjects+<>c
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "ParallelApisSubjects", "", 0u, 43u },
	{ "ParallelApisSubjects+<>c", "", 43u, 9u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[52] = {
	{ 0x0000000Cu, 0u },
	{ 0x0000000Du, 2u },
	{ 0x0000000Eu, 4u },
	{ 0x0000000Fu, 6u },
	{ 0x00000010u, 8u },
	{ 0x00000011u, 10u },
	{ 0x00000012u, 12u },
	{ 0x00000013u, 13u },
	{ 0x00000014u, 14u },
	{ 0x00000015u, 15u },
	{ 0x00000016u, 16u },
	{ 0x00000017u, 17u },
	{ 0x00000018u, 18u },
	{ 0x00000019u, 19u },
	{ 0x0000001Au, 20u },
	{ 0x0000001Bu, 21u },
	{ 0x0000001Cu, 22u },
	{ 0x0000001Du, 23u },
	{ 0x0000001Eu, 24u },
	{ 0x0000001Fu, 25u },
	{ 0x00000020u, 26u },
	{ 0x00000021u, 27u },
	{ 0x00000022u, 28u },
	{ 0x00000023u, 29u },
	{ 0x00000024u, 30u },
	{ 0x00000025u, 31u },
	{ 0x00000026u, 32u },
	{ 0x00000027u, 33u },
	{ 0x00000028u, 34u },
	{ 0x00000029u, 35u },
	{ 0x0000002Au, 36u },
	{ 0x0000002Bu, 37u },
	{ 0x0000002Cu, 38u },
	{ 0x0000002Du, 39u },
	{ 0x0000002Eu, 40u },
	{ 0x0000002Fu, 41u },
	{ 0x00000030u, 42u },
	{ 0x00000031u, 43u },
	{ 0x00000032u, 44u },
	{ 0x00000033u, 45u },
	{ 0x00000034u, 46u },
	{ 0x00000035u, 47u },
	{ 0x00000036u, 49u },
	{ 0x00000037u, 50u },
	{ 0x00000038u, 51u },
	{ 0x00000039u, 1u },
	{ 0x0000003Au, 3u },
	{ 0x0000003Bu, 5u },
	{ 0x0000003Cu, 7u },
	{ 0x0000003Du, 9u },
	{ 0x0000003Eu, 11u },
	{ 0x0000003Fu, 48u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[52] = {
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ParallelApisSubjects::Subject_0
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_0_b__1_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects+<>c::<Subject_0>b__1_0
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ParallelApisSubjects::Subject_1
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_1_b__2_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects+<>c::<Subject_1>b__2_0
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ParallelApisSubjects::Subject_2
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_2_b__3_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects+<>c::<Subject_2>b__3_0
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ParallelApisSubjects::Subject_3
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_3_b__4_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects+<>c::<Subject_3>b__4_0
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ParallelApisSubjects::Subject_4
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_4_b__5_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects+<>c::<Subject_4>b__5_0
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ParallelApisSubjects::Subject_5
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_5_b__6_0_System_Int32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects+<>c::<Subject_5>b__6_0
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::Subject_6
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::Subject_7
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_8
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_9
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_10
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_11
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_12
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_13
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_14
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_15
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_16
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_17
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_18
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_19), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_19
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_20), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_20
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_21), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_21
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_22), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_22
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_23), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_23
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_24), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_24
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_25), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_25
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_26), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_26
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_27), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_27
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_28), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_28
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_29), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_29
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_30), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_30
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_31), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_31
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_32
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_33), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_33
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_34), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_34
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_35), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_35
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_36), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_36
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_37), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_37
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_38), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_38
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_39), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_39
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_40), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::CustomEntrySubject_40
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_41), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ParallelApisSubjects::Subject_41
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_41_b__42_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects+<>c::<Subject_41>b__42_0
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_42), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects::Subject_42
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_cctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // ParallelApisSubjects+<>c::.cctor
	{ reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // ParallelApisSubjects+<>c::.ctor
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"ParallelApisSubjects",
	s_hotpatch_types,
	2u,
	s_hotpatch_methods,
	52u,
	s_hotpatch_slots,
	52u,
	s_hotpatch_entries,
	52u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[17] = {
	"ParallelApisSubjects/ParallelApisSubjects+<>c::<>9__1_0",
	"ParallelApisSubjects/ParallelApisSubjects+<>c::<>9",
	"System.Private.CoreLib/System.Action<System.Int32>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Action<System.Int32>)",
	"System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelLoopResult::GetHashCode:System.Int32()",
	"ParallelApisSubjects/ParallelApisSubjects::_exitCode",
	"ParallelApisSubjects/ParallelApisSubjects+<>c::<>9__2_0",
	"ParallelApisSubjects/ParallelApisSubjects+<>c::<>9__3_0",
	"System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::For:System.Threading.Tasks.ParallelLoopResult(System.Int32,System.Int32,System.Threading.Tasks.ParallelOptions,System.Action<System.Int32>)",
	"ParallelApisSubjects/ParallelApisSubjects+<>c::<>9__4_0",
	"ParallelApisSubjects/ParallelApisSubjects+<>c::<>9__5_0",
	"ParallelApisSubjects/ParallelApisSubjects+<>c::<>9__6_0",
	"System.Private.CoreLib/System.Action",
	"ParallelApisSubjects/ParallelApisSubjects+<>c::<>9__42_0",
	"System.Private.CoreLib/System.Action::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Threading.Tasks.Parallel/System.Threading.Tasks.Parallel::Invoke:System.Void(System.Action[])",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
};

extern "C" void* kChaosExternalRuntimeFnTable[17] = {
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
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
};

extern "C" int32_t kChaosExternalRuntimeCount = 17;
// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[52])() = {
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_Subject_0),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_0_b__1_0_System_Int32),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_Subject_1),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_1_b__2_0_System_Int32),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_Subject_2),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_2_b__3_0_System_Int32),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_Subject_3),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_3_b__4_0_System_Int32),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_Subject_4),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_4_b__5_0_System_Int32),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_Subject_5),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_5_b__6_0_System_Int32),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_Subject_6),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_Subject_7),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_8),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_9),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_10),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_11),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_12),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_13),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_14),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_15),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_16),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_17),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_18),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_19),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_20),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_21),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_22),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_23),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_24),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_25),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_26),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_27),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_28),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_29),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_30),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_31),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_32),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_33),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_34),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_35),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_36),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_37),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_38),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_39),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_40),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_Subject_41),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_41_b__42_0),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects_Subject_42),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects___c_cctor),
	reinterpret_cast<void(*)()>(&ParallelApisSubjects_ParallelApisSubjects___c__ctor),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[52])() = {
	[]() {kAotMethods[0]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[1])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[2]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[3])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[4]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[5])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[6]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[7])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[8]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[9])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
	[]() {kAotMethods[10]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kAotMethods[11])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel),0);},
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
	[]() {kAotMethods[22]();},
	[]() {kAotMethods[23]();},
	[]() {kAotMethods[24]();},
	[]() {kAotMethods[25]();},
	[]() {kAotMethods[26]();},
	[]() {kAotMethods[27]();},
	[]() {kAotMethods[28]();},
	[]() {kAotMethods[29]();},
	[]() {kAotMethods[30]();},
	[]() {kAotMethods[31]();},
	[]() {kAotMethods[32]();},
	[]() {kAotMethods[33]();},
	[]() {kAotMethods[34]();},
	[]() {kAotMethods[35]();},
	[]() {kAotMethods[36]();},
	[]() {kAotMethods[37]();},
	[]() {kAotMethods[38]();},
	[]() {kAotMethods[39]();},
	[]() {kAotMethods[40]();},
	[]() {kAotMethods[41]();},
	[]() {kAotMethods[42]();},
	[]() {kAotMethods[43]();},
	[]() {kAotMethods[44]();},
	[]() {kAotMethods[45]();},
	[]() {kAotMethods[46]();},
	[]() {kAotMethods[47]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[48])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
	[]() {kAotMethods[49]();},
	[]() {kAotMethods[50]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kAotMethods[51])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_benchmark_this_sentinel));},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 43;
extern "C" const int kSubjectEntryIndices[43] = {
	0,
	2,
	4,
	6,
	8,
	10,
	12,
	13,
	14,
	15,
	16,
	17,
	18,
	19,
	20,
	21,
	22,
	23,
	24,
	25,
	26,
	27,
	28,
	29,
	30,
	31,
	32,
	33,
	34,
	35,
	36,
	37,
	38,
	39,
	40,
	41,
	42,
	43,
	44,
	45,
	46,
	47,
	49,
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

	/* ── Entry 0: ParallelApisSubjects_ParallelApisSubjects_Subject_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry0;
	/* ── Entry 1: ParallelApisSubjects_ParallelApisSubjects___c_Subject_0_b__1_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry1;
	/* ── Entry 2: ParallelApisSubjects_ParallelApisSubjects_Subject_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry2;
	/* ── Entry 3: ParallelApisSubjects_ParallelApisSubjects___c_Subject_1_b__2_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry3;
	/* ── Entry 4: ParallelApisSubjects_ParallelApisSubjects_Subject_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry4;
	/* ── Entry 5: ParallelApisSubjects_ParallelApisSubjects___c_Subject_2_b__3_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry5;
	/* ── Entry 6: ParallelApisSubjects_ParallelApisSubjects_Subject_3 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry6;
	/* ── Entry 7: ParallelApisSubjects_ParallelApisSubjects___c_Subject_3_b__4_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry7;
	/* ── Entry 8: ParallelApisSubjects_ParallelApisSubjects_Subject_4 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry8;
	/* ── Entry 9: ParallelApisSubjects_ParallelApisSubjects___c_Subject_4_b__5_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry9;
	/* ── Entry 10: ParallelApisSubjects_ParallelApisSubjects_Subject_5 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry10;
	/* ── Entry 11: ParallelApisSubjects_ParallelApisSubjects___c_Subject_5_b__6_0_System_Int32 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry11;
	/* ── Entry 12: ParallelApisSubjects_ParallelApisSubjects_Subject_41 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry12;
	/* ── Entry 13: ParallelApisSubjects_ParallelApisSubjects___c_Subject_41_b__42_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry13;
	/* ── Entry 14: ParallelApisSubjects_ParallelApisSubjects___c_cctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry14;
	/* ── Entry 15: ParallelApisSubjects_ParallelApisSubjects___c__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry15;
} kChaosGcSlotMapsSection = {
	/* entry0 = ParallelApisSubjects_ParallelApisSubjects_Subject_0 */
	.entry0 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_0),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry1 = ParallelApisSubjects_ParallelApisSubjects___c_Subject_0_b__1_0_System_Int32 */
	.entry1 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_0_b__1_0_System_Int32),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry2 = ParallelApisSubjects_ParallelApisSubjects_Subject_1 */
	.entry2 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_1),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry3 = ParallelApisSubjects_ParallelApisSubjects___c_Subject_1_b__2_0_System_Int32 */
	.entry3 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_1_b__2_0_System_Int32),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry4 = ParallelApisSubjects_ParallelApisSubjects_Subject_2 */
	.entry4 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_2),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry5 = ParallelApisSubjects_ParallelApisSubjects___c_Subject_2_b__3_0_System_Int32 */
	.entry5 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_2_b__3_0_System_Int32),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry6 = ParallelApisSubjects_ParallelApisSubjects_Subject_3 */
	.entry6 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_3),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry7 = ParallelApisSubjects_ParallelApisSubjects___c_Subject_3_b__4_0_System_Int32 */
	.entry7 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_3_b__4_0_System_Int32),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry8 = ParallelApisSubjects_ParallelApisSubjects_Subject_4 */
	.entry8 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_4),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry9 = ParallelApisSubjects_ParallelApisSubjects___c_Subject_4_b__5_0_System_Int32 */
	.entry9 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_4_b__5_0_System_Int32),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry10 = ParallelApisSubjects_ParallelApisSubjects_Subject_5 */
	.entry10 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_5),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry11 = ParallelApisSubjects_ParallelApisSubjects___c_Subject_5_b__6_0_System_Int32 */
	.entry11 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_5_b__6_0_System_Int32),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry12 = ParallelApisSubjects_ParallelApisSubjects_Subject_41 */
	.entry12 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_41),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry13 = ParallelApisSubjects_ParallelApisSubjects___c_Subject_41_b__42_0 */
	.entry13 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_41_b__42_0),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry14 = ParallelApisSubjects_ParallelApisSubjects___c_cctor */
	.entry14 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&ParallelApisSubjects_ParallelApisSubjects___c_cctor),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry15 = ParallelApisSubjects_ParallelApisSubjects___c__ctor */
	.entry15 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&ParallelApisSubjects_ParallelApisSubjects___c__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 512u;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[52] = {
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_0),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_0_b__1_0_System_Int32),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_1),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_1_b__2_0_System_Int32),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_2),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_2_b__3_0_System_Int32),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_3),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_3_b__4_0_System_Int32),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_4),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_4_b__5_0_System_Int32),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_5),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_5_b__6_0_System_Int32),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_6),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_7),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_8),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_9),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_10),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_11),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_12),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_13),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_14),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_15),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_16),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_17),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_18),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_19),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_20),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_21),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_22),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_23),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_24),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_25),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_26),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_27),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_28),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_29),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_30),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_31),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_32),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_33),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_34),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_35),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_36),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_37),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_38),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_39),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_40),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_41),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_Subject_41_b__42_0),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects_Subject_42),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c_cctor),
	reinterpret_cast<void*>(&ParallelApisSubjects_ParallelApisSubjects___c__ctor),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0xEA63C56EBA9A02E0),
		0x00000002u,
		0u,
		8u,
		reinterpret_cast<const void*>(kSlots_ParallelApisSubjects_ParallelApisSubjects___c),
		reinterpret_cast<const void**>(chaos_vtable_ParallelApisSubjects_ParallelApisSubjects___c),
		8u,
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
	.method_pointer_count      = 52u,
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
	.image_name_utf8    = "ParallelApisSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_ParallelApisSubjects_ParallelApisSubjects[1] = {
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_ParallelApisSubjects_ParallelApisSubjects[43] = {
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::Subject_0:System.Void()", "Subject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::Subject_1:System.Void()", "Subject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::Subject_2:System.Void()", "Subject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::Subject_3:System.Void()", "Subject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::Subject_4:System.Void()", "Subject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::Subject_5:System.Void()", "Subject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::Subject_6:System.Void()", "Subject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::Subject_7:System.Void()", "Subject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_9:System.Void()", "CustomEntrySubject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_10:System.Void()", "CustomEntrySubject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_11:System.Void()", "CustomEntrySubject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_12:System.Void()", "CustomEntrySubject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_13:System.Void()", "CustomEntrySubject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_14:System.Void()", "CustomEntrySubject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_15:System.Void()", "CustomEntrySubject_15", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_16:System.Void()", "CustomEntrySubject_16", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_17:System.Void()", "CustomEntrySubject_17", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_18:System.Void()", "CustomEntrySubject_18", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_19:System.Void()", "CustomEntrySubject_19", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_20:System.Void()", "CustomEntrySubject_20", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_21:System.Void()", "CustomEntrySubject_21", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_22:System.Void()", "CustomEntrySubject_22", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_23:System.Void()", "CustomEntrySubject_23", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_24:System.Void()", "CustomEntrySubject_24", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_25:System.Void()", "CustomEntrySubject_25", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_26:System.Void()", "CustomEntrySubject_26", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_27:System.Void()", "CustomEntrySubject_27", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_28:System.Void()", "CustomEntrySubject_28", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_29:System.Void()", "CustomEntrySubject_29", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_30:System.Void()", "CustomEntrySubject_30", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_31:System.Void()", "CustomEntrySubject_31", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_32:System.Void()", "CustomEntrySubject_32", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_33:System.Void()", "CustomEntrySubject_33", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_34:System.Void()", "CustomEntrySubject_34", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_35:System.Void()", "CustomEntrySubject_35", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_36:System.Void()", "CustomEntrySubject_36", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_37:System.Void()", "CustomEntrySubject_37", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_38:System.Void()", "CustomEntrySubject_38", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_39:System.Void()", "CustomEntrySubject_39", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_40:System.Void()", "CustomEntrySubject_40", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::Subject_41:System.Void()", "Subject_41", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects::Subject_42:System.Void()", "Subject_42", "System.Void", 0, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_ParallelApisSubjects_ParallelApisSubjects___c[9] = {
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects+<>c::<Subject_0>b__1_0:System.Void(System.Int32)", "<Subject_0>b__1_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects+<>c::<Subject_1>b__2_0:System.Void(System.Int32)", "<Subject_1>b__2_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects+<>c::<Subject_2>b__3_0:System.Void(System.Int32)", "<Subject_2>b__3_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects+<>c::<Subject_3>b__4_0:System.Void(System.Int32)", "<Subject_3>b__4_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects+<>c::<Subject_4>b__5_0:System.Void(System.Int32)", "<Subject_4>b__5_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects+<>c::<Subject_5>b__6_0:System.Void(System.Int32)", "<Subject_5>b__6_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects+<>c::<Subject_41>b__42_0:System.Void()", "<Subject_41>b__42_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects+<>c::.cctor:System.Void()", ".cctor", "System.Void", 0, nullptr, 0u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects+<>c::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects", "ParallelApisSubjects/ParallelApisSubjects", "", "ParallelApisSubjects", "ParallelApisSubjects", nullptr, kReflFields_ParallelApisSubjects_ParallelApisSubjects, 1u, nullptr, 0u,
	kReflMethods_ParallelApisSubjects_ParallelApisSubjects, 43u },
	{ 0u, "ParallelApisSubjects/ParallelApisSubjects+<>c", "ParallelApisSubjects/ParallelApisSubjects+<>c", "", "ParallelApisSubjects+<>c", "ParallelApisSubjects+<>c", nullptr, nullptr, 0u, nullptr, 0u,
	kReflMethods_ParallelApisSubjects_ParallelApisSubjects___c, 9u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

static const ReflectionQueryImageDescriptor kReflImage = { "ParallelApisSubjects", kReflTypePtrs, 2u };

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
// Managed method: ParallelApisSubjects/ParallelApisSubjects::Subject_0()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_0(void)
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
	_s2 = chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__1_0;
	_s3 = _s2;
	{
		if (_s3 == 0)
		{
			chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
			_s2 = chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 1)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> void {
					auto& _d_entry = s_hotpatch_entries[1];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, nullptr);
						return;
					}
					reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s3;
				const auto chaos_target = _s2;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Action_System_Int32_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s3 = _s2;
			{
				auto chaos_value = _s3;
			chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
				chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__1_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_2 = _s2;
			const auto chaos_arg_1 = _s1;
			const auto chaos_arg_0 = _s0;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[1] = _s0;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[4])();
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(410570569);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
		{
			if (_s1 != 0)
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s1;
					chaos_static_ParallelApisSubjects_ParallelApisSubjects___exitCode = chaos_value;
				}
			}
			return;
		}
	}
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects+<>c::<Subject_0>b__1_0(System.Int32)
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_0_b__1_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::Subject_1()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_1(void)
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
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
	_s2 = chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__2_0;
	_s3 = _s2;
	{
		if (_s3 == 0)
		{
			chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
			_s2 = chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 3)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> void {
					auto& _d_entry = s_hotpatch_entries[3];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[12];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, nullptr);
						return;
					}
					reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
				};
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s3;
				const auto chaos_target = _s2;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action_System_Int32_, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Action_System_Int32_.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s3 = _s2;
			{
				auto chaos_value = _s3;
			chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
				chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__2_0 = chaos_value;
			}
		}
		{
			const auto chaos_arg_2 = _s2;
			const auto chaos_arg_1 = _s1;
			const auto chaos_arg_0 = _s0;
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[1] = _s0;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
		{
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[4])();
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(410570569);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
		{
			if (_s1 != 0)
			{
				_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s1;
					chaos_static_ParallelApisSubjects_ParallelApisSubjects___exitCode = chaos_value;
				}
			}
			return;
		}
	}
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects+<>c::<Subject_1>b__2_0(System.Int32)
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_1_b__2_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::Subject_2()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_2(void)
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


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = 0;
			chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
			_s3 = chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__3_0;
			_s4 = _s3;
			{
				if (_s4 == 0)
				{
					chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
					_s3 = chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9;
					{
						// Hotpatch-aware ldftn wrapper (slot 5)
						static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> void {
							auto& _d_entry = s_hotpatch_entries[5];
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
							{
								alignas(16) uint8_t _d_ab[12];
								ArgBuffer _d_bw(_d_ab);
								_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
								_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d_entry.method_key, _d_ab, nullptr);
								return;
							}
							reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
						};
					_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
					}
					{
						const auto chaos_method_ptr = _s4;
						const auto chaos_target = _s3;
						auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action_System_Int32_, {});
						chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Action_System_Int32_.hot;
						chaos_object->chaos_delegate_target = chaos_target;
						chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
						_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
					}
					_s4 = _s3;
					{
						auto chaos_value = _s4;
					chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
						chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__3_0 = chaos_value;
					}
				}
				{
					const auto chaos_arg_3 = _s3;
					const auto chaos_arg_2 = _s2;
					const auto chaos_arg_1 = _s1;
					const auto chaos_arg_0 = _s0;
					const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[8])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s0;
					chaos_static_ParallelApisSubjects_ParallelApisSubjects___exitCode = chaos_value;
				}
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_ArgumentNullException.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
	CHAOS_EH_END
	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects+<>c::<Subject_2>b__3_0(System.Int32)
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_2_b__3_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::Subject_3()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_3(void)
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


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = 0;
			chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
			_s3 = chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__4_0;
			_s4 = _s3;
			{
				if (_s4 == 0)
				{
					chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
					_s3 = chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9;
					{
						// Hotpatch-aware ldftn wrapper (slot 7)
						static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> void {
							auto& _d_entry = s_hotpatch_entries[7];
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
							{
								alignas(16) uint8_t _d_ab[12];
								ArgBuffer _d_bw(_d_ab);
								_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
								_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d_entry.method_key, _d_ab, nullptr);
								return;
							}
							reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
						};
					_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
					}
					{
						const auto chaos_method_ptr = _s4;
						const auto chaos_target = _s3;
						auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action_System_Int32_, {});
						chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Action_System_Int32_.hot;
						chaos_object->chaos_delegate_target = chaos_target;
						chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
						_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
					}
					_s4 = _s3;
					{
						auto chaos_value = _s4;
					chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
						chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__4_0 = chaos_value;
					}
				}
				{
					const auto chaos_arg_3 = _s3;
					const auto chaos_arg_2 = _s2;
					const auto chaos_arg_1 = _s1;
					const auto chaos_arg_0 = _s0;
					const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[8])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s0;
					chaos_static_ParallelApisSubjects_ParallelApisSubjects___exitCode = chaos_value;
				}
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_ArgumentNullException.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
	CHAOS_EH_END
	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects+<>c::<Subject_3>b__4_0(System.Int32)
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_3_b__4_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::Subject_4()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_4(void)
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


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = 0;
			chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
			_s3 = chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__5_0;
			_s4 = _s3;
			{
				if (_s4 == 0)
				{
					chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
					_s3 = chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9;
					{
						// Hotpatch-aware ldftn wrapper (slot 9)
						static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> void {
							auto& _d_entry = s_hotpatch_entries[9];
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
							{
								alignas(16) uint8_t _d_ab[12];
								ArgBuffer _d_bw(_d_ab);
								_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
								_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d_entry.method_key, _d_ab, nullptr);
								return;
							}
							reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
						};
					_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
					}
					{
						const auto chaos_method_ptr = _s4;
						const auto chaos_target = _s3;
						auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action_System_Int32_, {});
						chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Action_System_Int32_.hot;
						chaos_object->chaos_delegate_target = chaos_target;
						chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
						_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
					}
					_s4 = _s3;
					{
						auto chaos_value = _s4;
					chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
						chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__5_0 = chaos_value;
					}
				}
				{
					const auto chaos_arg_3 = _s3;
					const auto chaos_arg_2 = _s2;
					const auto chaos_arg_1 = _s1;
					const auto chaos_arg_0 = _s0;
					const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[8])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s0;
					chaos_static_ParallelApisSubjects_ParallelApisSubjects___exitCode = chaos_value;
				}
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_ArgumentNullException.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
	CHAOS_EH_END
	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects+<>c::<Subject_4>b__5_0(System.Int32)
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_4_b__5_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::Subject_5()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_5(void)
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


	CHAOS_EH_TRY
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
			_s2 = 0;
			chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
			_s3 = chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__6_0;
			_s4 = _s3;
			{
				if (_s4 == 0)
				{
					chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
					_s3 = chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9;
					{
						// Hotpatch-aware ldftn wrapper (slot 11)
						static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1) -> void {
							auto& _d_entry = s_hotpatch_entries[11];
							if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
								&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
							{
								alignas(16) uint8_t _d_ab[12];
								ArgBuffer _d_bw(_d_ab);
								_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
								_d_bw.WriteI32(static_cast<CHAOS_IL2CPP_INT32>(chaos_fn_arg_1));
								::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
									_d_entry.method_key, _d_ab, nullptr);
								return;
							}
							reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1);
						};
					_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
					}
					{
						const auto chaos_method_ptr = _s4;
						const auto chaos_target = _s3;
						auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action_System_Int32_, {});
						chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Action_System_Int32_.hot;
						chaos_object->chaos_delegate_target = chaos_target;
						chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
						_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
					}
					_s4 = _s3;
					{
						auto chaos_value = _s4;
					chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
						chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__6_0 = chaos_value;
					}
				}
				{
					const auto chaos_arg_3 = _s3;
					const auto chaos_arg_2 = _s2;
					const auto chaos_arg_1 = _s1;
					const auto chaos_arg_0 = _s0;
					const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[8])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
				{
					auto chaos_value = _s0;
					chaos_static_ParallelApisSubjects_ParallelApisSubjects___exitCode = chaos_value;
				}
			}
	CHAOS_EH_CATCH_BEGIN
		if (CHAOS_EH_EXCEPTION_OBJ < 0) { CHAOS_EH_RETHROW; }
		auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(CHAOS_EH_EXCEPTION_OBJ);
		if (chaos_header != nullptr)
		{
			if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), &chaos_mt_System_Private_CoreLib_System_ArgumentNullException.hot))
			{ CHAOS_EH_RETHROW; }
		}
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
	CHAOS_EH_END
	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects+<>c::<Subject_5>b__6_0(System.Int32)
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_5_b__6_0_System_Int32(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);

	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::Subject_6()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::Subject_7()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_8()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_9()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_10()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_11()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_12()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_13()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_14()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_14(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_15()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_15(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_16()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_16(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_17()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_17(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_18()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_18(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_19()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_19(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_20()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_20(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_21()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_21(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_22()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_22(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_23()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_23(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_24()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_24(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_25()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_25(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_26()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_26(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_27()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_27(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_28()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_28(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_29()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_29(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_30()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_30(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_31()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_31(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_32()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_32(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_33()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_33(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_34()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_34(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_35()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_35(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_36()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_36(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_37()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_37(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_38()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_38(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_39()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_39(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::CustomEntrySubject_40()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_CustomEntrySubject_40(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::Subject_41()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_41(void)
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


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(_s0);
		if (chaos_length < 0)
		{
			CHAOS_IL2CPP_FAIL();
		}
		auto* chaos_array = CHAOS_IL2CPP_NEW_GC(chaos_managed_array, {});
		chaos_array->header.type_info = &chaos_type_info_managed_array.hot;
		chaos_array->element_type_shape = 1;
		chaos_array->element_type_info = &chaos_mt_System_Private_CoreLib_System_Action.hot;
		chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);
		chaos_array->elements = chaos_length == 0 ? nullptr : CHAOS_IL2CPP_NEW_GC_ARRAY(CHAOS_IL2CPP_INTPTR, static_cast<CHAOS_IL2CPP_SIZE>(chaos_length));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);
	}
	_s1 = _s0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
	_s3 = chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__42_0;
	_s4 = _s3;
	{
		if (_s4 == 0)
		{
			chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
			_s3 = chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 48)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[48];
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
			_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s4;
				const auto chaos_target = _s3;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action, {});
				chaos_object->header.type_info = &chaos_mt_System_Private_CoreLib_System_Action.hot;
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s4 = _s3;
			{
				auto chaos_value = _s4;
			chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
				chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9__42_0 = chaos_value;
			}
		}
		{
			auto chaos_value_raw = _s4;
			auto chaos_value = chaos_value_raw;
			const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s3);
			auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s2);
			if (chaos_array == nullptr)
			{
				CHAOS_IL2CPP_FAIL();
			}
			if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
			{
				CHAOS_IL2CPP_FAIL();
			}
			if (!chaos_is_array_store_compatible(chaos_array, chaos_value))
			{
				CHAOS_IL2CPP_FAIL();
			}
			BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]));
			chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;
			GC_END_STUBBORN_CHANGE(chaos_array);
			chaos_gc_dirty_card(chaos_array);
		}
		{
			const auto chaos_arg_0 = _s1;
			reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_arg_0);
		}
		return;
	}
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects+<>c::<Subject_41>b__42_0()
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_Subject_41_b__42_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects::Subject_42()
extern "C" void ParallelApisSubjects_ParallelApisSubjects_Subject_42(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects+<>c::.cctor()
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c_cctor(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};

	chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();

	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_ParallelApisSubjects_ParallelApisSubjects___c, {});
		chaos_object->header.type_info = &chaos_mt_ParallelApisSubjects_ParallelApisSubjects___c.hot;
		ParallelApisSubjects_ParallelApisSubjects___c__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
	chaos_ensure_type_initialized_ParallelApisSubjects_ParallelApisSubjects___c();
		chaos_static_ParallelApisSubjects_ParallelApisSubjects___c____9 = chaos_value;
	}
	return;
}

// Managed method: ParallelApisSubjects/ParallelApisSubjects+<>c::.ctor()
extern "C" void ParallelApisSubjects_ParallelApisSubjects___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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



}  // namespace chaos::il2cpp::codegen::ParallelApisSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 52;

extern "C" void ChaosJitRegisterAll() {}