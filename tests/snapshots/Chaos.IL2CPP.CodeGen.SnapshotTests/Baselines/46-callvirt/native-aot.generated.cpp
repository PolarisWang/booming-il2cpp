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
#include "native-aot.generated.header.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

namespace chaos::il2cpp::codegen::SnapshotTestFixtures {

// Bring runtime_core and jit declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::jit;

extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CallvirtHelper_RunCallvirt(void);
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_InstanceHelper_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);

// Runtime prelude dependencies included at translation unit level


// ── Virtual method table arrays ──
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_InstanceHelper_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
const void* chaos_vtable_SnapshotTestFixtures_InstanceHelper[] =
{
	reinterpret_cast<void*>(SnapshotTestFixtures_InstanceHelper_GetValue),
};
inline MethodTable chaos_mt_SnapshotTestFixtures_InstanceHelper = {nullptr, chaos_vtable_SnapshotTestFixtures_InstanceHelper, 7539435265304875683ULL, 1u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_SnapshotTestFixtures_InstanceHelper = static_cast<CHAOS_IL2CPP_UINT64>(7539435265304875683ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Object = {nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_UINT64>(15228727185366376748ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_UINT64>(5474029880995115448ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_UINT64>(17082367815459723707ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 4197398748135124456ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_MethodBase = static_cast<CHAOS_IL2CPP_UINT64>(4197398748135124456ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase), nullptr, 4137207361503509124ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_UINT64>(4137207361503509124ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_UINT64>(17040031516751226236ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase), nullptr, 10748947813473285525ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_UINT64>(10748947813473285525ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_String = {nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_UINT64>(1782325859292956794ULL);
inline MethodTable chaos_mt_System_Private_CoreLib_System_Type = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 3222174513575444759ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Type = static_cast<CHAOS_IL2CPP_UINT64>(3222174513575444759ULL);

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_SnapshotTestFixtures_InstanceHelper[] =
{
	{ 0x00000004u, reinterpret_cast<void*>(&SnapshotTestFixtures_InstanceHelper_GetValue) },
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

struct chaos_type_SnapshotTestFixtures_InstanceHelper
{
	ThinLockableHeader header{};
};

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

struct chaos_type_System_Private_CoreLib_System_Reflection_MethodBase : public chaos_type_System_Private_CoreLib_System_Object
{
};

struct chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo : public chaos_type_System_Private_CoreLib_System_Reflection_MethodBase
{
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

struct chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo : public chaos_type_System_Private_CoreLib_System_Reflection_MethodBase
{
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

struct chaos_type_System_Private_CoreLib_System_Type : public chaos_type_System_Private_CoreLib_System_Object
{
	CHAOS_IL2CPP_INTPTR runtime_type_handle = 0;
	CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
	CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};


// ── GcTypeLayout pointer offset arrays (computed via offsetof) ──
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Type[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_name_value))};

extern "C" void ChaosRegisterGcLayouts() {
	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();

	// Register managed_array (variable-size: header + contiguous element data).
	registry.Register(CHAOS_IL2CPP_UINT64(chaos_type_id_managed_array), sizeof(chaos_managed_array), nullptr, 0,
		static_cast<uint16_t>(sizeof(CHAOS_IL2CPP_INTPTR)),
		static_cast<uint16_t>(offsetof(chaos_managed_array, length)));

	registry.Register(7539435265304875683ULL, sizeof(chaos_type_SnapshotTestFixtures_InstanceHelper), nullptr, 0);
	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4197398748135124456ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodBase), nullptr, 0);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);
	registry.Register(3222174513575444759ULL, sizeof(chaos_type_System_Private_CoreLib_System_Type), kGcOffsets_chaos_type_System_Private_CoreLib_System_Type, 2);

	// Register MethodTable address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_InstanceHelper), reinterpret_cast<uintptr_t>(&chaos_mt_SnapshotTestFixtures_InstanceHelper) + sizeof(chaos_mt_SnapshotTestFixtures_InstanceHelper));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Type), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Type) + sizeof(chaos_mt_System_Private_CoreLib_System_Type));
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[3] = {
	0u,
	0u,
	1u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[2];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		2u,
		1u,
		3796745736u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 1u, 0u },  // SnapshotTestFixtures_CallvirtHelper_RunCallvirt
		{ 1u, 1u },  // SnapshotTestFixtures_InstanceHelper_GetValue
	},
	{
		2u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[108] = {
		0u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1536u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		12u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1728u,
		1548u,
		1536u,
		1536u,
		1548u,
		1556u,
		1536u,
		1548u,
		1545u,
		1548u,
		1548u,
		1548u,
		1548u,
		1536u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1545u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1536u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1536u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1536u,
		1548u,
		1556u,
		1536u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
		1548u,
	};

	static constexpr const char* s_type_names[108] = {
		"<Module>",
		"ArithmeticOps",
		"StringOps",
		"VoidCaller",
		"MathHelper",
		"FlowControl",
		"InstanceHelper",
		"CallVirtHelper",
		"BoxingHelper",
		"FieldHelper",
		"LoopHelper",
		"ConversionHelper",
		"Helper",
		"SwitchHelper",
		"LdftnHelper",
		"TargetHelper",
		"ArrayHelper",
		"EhHelper",
		"GenericHelper",
		"Wrapper`1",
		"DelegateHelper",
		"BaseClass",
		"DerivedClass",
		"VirtualHelper",
		"IMarker",
		"MarkedClass",
		"TypeCheckHelper",
		"SmallStruct",
		"SizeOfHelper",
		"IndirectHelper",
		"ArithmeticCompareHelper",
		"BranchDupHelper",
		"HasInstanceFields",
		"InstanceFieldHelper",
		"StaticFieldWriteHelper",
		"VirtualDispatchHelper",
		"BitwiseHelper",
		"ShiftHelper",
		"ArrayLengthHelper",
		"MyValueType",
		"ValueTypeHelper",
		"LocalAllocHelper",
		"MulDivHelper",
		"BranchCompareHelper",
		"FloatOpsHelper",
		"ConvWideHelper",
		"NegShiftHelper",
		"LdcI8Helper",
		"ConvSmallIntHelper",
		"LdindI1Helper",
		"BranchCompareBHelper",
		"HasFields",
		"AddressHelper",
		"LdtokenHelper",
		"LdelemaHelper",
		"LdargaHelper",
		"CpobjHelper",
		"StindNarrowHelper",
		"CallvirtHelper",
		"UnboxHelper",
		"CpblkHelper",
		"LdindUnsignedHelper",
		"LdindI8Helper",
		"LdindWideHelper",
		"StindWideHelper",
		"ArrayRefHelper",
		"ThrowHelper",
		"OverflowHelper",
		"OverflowConvHelper",
		"CalliHelper",
		"MyClass",
		"LdVirtftnHelper",
		"LdelemAllHelper",
		"StelemAllHelper",
		"UnsignedOpsHelper",
		"RareOpsHelper",
		"StringFormatHelper",
		"BranchUnsignedHelper",
		"BranchUnsignedBHelper",
		"RethrowHelper",
		"OverflowUnHelper",
		"MoreRareOpsHelper",
		"OverflowConvExtHelper",
		"SimpleGapsHelper",
		"LdindStindGapsHelper",
		"CollectionsHelper",
		"ExternalCaller",
		"MultiDispatcher",
		"InternalHelper",
		"StringConcatHelper",
		"GenericsVirtEhDemo",
		"BoxInterfaceArrayDemo",
		"HotUpdateWithTypesDemo",
		"HotUpdateWithGenericsDemo",
		"SealedHelper",
		"SealedClassVirtualHelper",
		"IMyInterface",
		"ImplHelper",
		"InterfaceDevirtHelper",
		"EhFinallyHelper",
		"EhFaultHelper",
		"EhFilterHelper",
		"EhMultipleCatchHelper",
		"EhCatchFinallyNestedHelper",
		"EhFilterFinallyHelper",
		"EhNestedTryCatchHelper",
		"EhFaultFinallyHelper",
		"<>O",
	};

	static constexpr const char* s_type_namespaces[108] = {
		"",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"SnapshotTestFixtures",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[108] = {
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
		33554454u,
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

	static const TypeInfoHot* const s_type_info_ptrs[108] = {
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

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[1] = {
		33554540u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[109] = {
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
		1u,
		1u,
		1u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[109] = {
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



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "SnapshotTestFixtures",
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
		/* .type_count        = */ 108u,
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
		::chaos::il2cpp::runtime_core::RegisterModule("SnapshotTestFixtures", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[2] = {
	{ "RunCallvirt", 0x00000002u, 0u },  // CallvirtHelper
	{ "GetValue", 0x00000004u, 0u },  // InstanceHelper
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[2] = {
	{ "CallvirtHelper", "", 0u, 1u },
	{ "InstanceHelper", "", 1u, 1u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[2] = {
	{ 0x00000002u, 0u },
	{ 0x00000004u, 1u },
};

// Dispatch table (function pointers)
// s_hotpatch_entries — external linkage for verification_dispatch
// NOTE: `static` is deliberately omitted so that the verification dispatch
// TU (verification_dispatch.generated.cpp) can access this array via
// GetHotpatchEntries(). Without external linkage, the linker cannot resolve
// the symbol across translation units.
extern "C" HotpatchEntryV0 s_hotpatch_entries[2] = {
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_CallvirtHelper_RunCallvirt), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // CallvirtHelper::RunCallvirt
	{ reinterpret_cast<void*>(&SnapshotTestFixtures_InstanceHelper_GetValue), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // InstanceHelper::GetValue
};

// Accessor functions for verification_dispatch.generated.cpp
extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept { return s_hotpatch_entries; }
extern "C" int32_t GetHotpatchEntryCount() noexcept { return 2; }

// Module hotpatch bundle (extern linkage — referenced across page files)
extern constexpr HotpatchModuleV0 s_hotpatch_module = {
	"SnapshotTestFixtures",
	s_hotpatch_types,
	2u,
	s_hotpatch_methods,
	2u,
	s_hotpatch_slots,
	2u,
	s_hotpatch_entries,
	2u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[1] = {
	"SnapshotTestFixtures/InstanceHelper::.ctor:System.Void(System.Int32)",
};

extern "C" void* kChaosExternalRuntimeFnTable[1] = {
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 1;
// (no method AOT entries for this module)
// ── Method table (kMethodTable[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
// NOTE: Pure data only — dispatch routing is in <chaos/hotpatch_dispatch.h>.
// NOTE: Emitted in ALL modes (AOT + JIT). JIT mode verification dispatch
// uses kDefaultArgThunks to bypass JIT precode trampoline (verification
// tests AOT codegen, not JIT compilation).
static void (*kMethodTable[2])() = {
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_CallvirtHelper_RunCallvirt),
	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_InstanceHelper_GetValue),
};

// ── Default-arg thunks (kDefaultArgThunks[]) ────────────────────────────
// Each thunk supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_null_instance = 0;
static void (*kDefaultArgThunks[2])() = {
	[]() {kMethodTable[0]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[1])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
};

// ── Subject slot map (empty) ────────────────────────────────────────
// No subject entries match the ::Subject_N / ::CustomEntrySubject_N
// pattern, so the map is empty. kSubjectEntryCount is still defined as 0
// so runtime-entry.cpp / verification_dispatch.generated.cpp can link.
extern "C" const int kSubjectEntryCount = 0;
extern "C" const int kSubjectSlotMap[1] = { 0 };
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

	/* ── Entry 0: SnapshotTestFixtures_CallvirtHelper_RunCallvirt ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry0;
	/* ── Entry 1: SnapshotTestFixtures_InstanceHelper_GetValue ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry1;
} kChaosGcSlotMapsSection = {
	/* entry0 = SnapshotTestFixtures_CallvirtHelper_RunCallvirt */
	.entry0 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_CallvirtHelper_RunCallvirt),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry1 = SnapshotTestFixtures_InstanceHelper_GetValue */
	.entry1 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_InstanceHelper_GetValue),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 56u;

// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: SnapshotTestFixtures/InstanceHelper::.ctor:System.Void(System.Int32)
extern "C" void chaos_bridge_thunk_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[0])(chaos_fn_arg_0);
}

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[2] = {
	reinterpret_cast<void*>(&SnapshotTestFixtures_CallvirtHelper_RunCallvirt),
	reinterpret_cast<void*>(&SnapshotTestFixtures_InstanceHelper_GetValue),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0x68A1746D9B8CD2A3),
		0x00000003u,
		0u,
		1u,
		reinterpret_cast<const void*>(kSlots_SnapshotTestFixtures_InstanceHelper),
		reinterpret_cast<const void**>(chaos_vtable_SnapshotTestFixtures_InstanceHelper),
		1u,
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
	.method_pointer_count      = 2u,
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
	.image_name_utf8    = "SnapshotTestFixtures",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_CallvirtHelper[1] = {
	{ 0x00000002u, "SnapshotTestFixtures/CallvirtHelper::RunCallvirt:System.Int32()", "RunCallvirt", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_InstanceHelper[1] = {
	{ 0x00000004u, "SnapshotTestFixtures/InstanceHelper::GetValue:System.Int32()", "GetValue", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[2] = {
	{ 0x00000001u, "SnapshotTestFixtures/CallvirtHelper", "SnapshotTestFixtures/CallvirtHelper", "", "CallvirtHelper", "CallvirtHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_CallvirtHelper, 1u, nullptr, 0u, 0u },
	{ 0x00000003u, "SnapshotTestFixtures/InstanceHelper", "SnapshotTestFixtures/InstanceHelper", "", "InstanceHelper", "InstanceHelper", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_SnapshotTestFixtures_InstanceHelper, 1u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[2] = {
	&kReflTypes[0],
	&kReflTypes[1],
};

extern const ReflectionQueryImageDescriptor kReflImage = { "SnapshotTestFixtures", kReflTypePtrs, 2u, 1, 0, 0, 0 };

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
// AOT-unreachable stub: SnapshotTestFixtures/CallvirtHelper::RunCallvirt:System.Int32()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CallvirtHelper_RunCallvirt(void)
{
	return {};
}

// Managed method: SnapshotTestFixtures/InstanceHelper::GetValue()
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_InstanceHelper_GetValue(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}



}  // namespace chaos::il2cpp::codegen::SnapshotTestFixtures
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 2;

extern "C" void ChaosJitRegisterAll() {}

// Forward declaration for entry symbol (defined in codegen namespace above).
extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_CallvirtHelper_RunCallvirt(CHAOS_IL2CPP_INT32);
extern "C" CHAOS_IL2CPP_INT32 RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex) {
    return SnapshotTestFixtures_CallvirtHelper_RunCallvirt(entryIndex);
}