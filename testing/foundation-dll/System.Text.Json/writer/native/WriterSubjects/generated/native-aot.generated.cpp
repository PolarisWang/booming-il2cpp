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

namespace chaos::il2cpp::codegen::WriterSubjects {

// Bring runtime_core declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;

// Runtime prelude dependencies included at translation unit level


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

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_0(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_1(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_2(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_3(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_4(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_5(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_6(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_7(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_8(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_9(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_10(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_11(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_12(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_13(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_14(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_15(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_16(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_17(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_18(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_19(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_20(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_21(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_22(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_23(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_24(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_25(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_26(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_27(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_28(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_29(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_30(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_31(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_32(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_33(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_34(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_35(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_36(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_37(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_38(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_39(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_40(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_41(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_42(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_43(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_44(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_45(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_46(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_47(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_48(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_49(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_50(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_51(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_52(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_53(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_54(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_55(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_56(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_57(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_58(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_59(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_60(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_61(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_62(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_63(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_64(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_65(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_66(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_67(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_68(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_69(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_70(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_71(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_72(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_73(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_74(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_75(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_76(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_77(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_78(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_79(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_80(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_81(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_82(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_83(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_84(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_85(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_86(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_87(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_88(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_89(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_90(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_91(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_92(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_93(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_94(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_95(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_96(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_97(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_98(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_99(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_100(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_101(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_102(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_103(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_104(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_105(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_106(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_107(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_108(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_109(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_110(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_111(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_112(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_113(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_114(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_115(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_116(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_117(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_118(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_119(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_120(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_121(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_122(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_123(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_124(void);
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_125(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[127] = {
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
	::ChaosAbiMethodEntryV0 entries[126];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		126u,
		0u,
		1564938101u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_0
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_1
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_2
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_3
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_4
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_5
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_6
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_7
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_8
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_9
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_10
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_11
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_12
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_13
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_14
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_15
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_16
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_17
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_18
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_19
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_20
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_21
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_22
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_23
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_24
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_25
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_26
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_27
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_28
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_29
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_30
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_31
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_32
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_33
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_34
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_35
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_36
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_37
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_38
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_39
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_40
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_41
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_42
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_43
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_44
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_45
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_46
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_47
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_48
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_49
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_50
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_51
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_52
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_53
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_54
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_55
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_56
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_57
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_58
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_59
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_60
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_61
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_62
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_63
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_64
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_65
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_66
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_67
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_68
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_69
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_70
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_71
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_72
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_73
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_74
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_75
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_76
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_77
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_78
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_79
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_80
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_81
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_82
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_83
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_84
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_85
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_86
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_87
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_88
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_89
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_90
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_91
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_92
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_93
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_94
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_95
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_96
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_97
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_98
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_99
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_100
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_101
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_102
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_103
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_104
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_105
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_106
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_107
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_108
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_109
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_110
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_111
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_112
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_113
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_114
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_115
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_116
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_117
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_118
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_119
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_120
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_121
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_122
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_123
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_124
		{ 0u, 0u },  // WriterSubjects_WriterSubjects_CustomEntrySubject_125
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
		"WriterSubjects",
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
		nullptr,
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
		/* .name_utf8         = */ "WriterSubjects",
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
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("WriterSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[126] = {
	{ "CustomEntrySubject_0", 0x00000003u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_1", 0x00000004u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_2", 0x00000005u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_3", 0x00000006u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_4", 0x00000007u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_5", 0x00000008u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_6", 0x00000009u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_7", 0x0000000Au, 0u },  // WriterSubjects
	{ "CustomEntrySubject_8", 0x0000000Bu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_9", 0x0000000Cu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_10", 0x0000000Du, 0u },  // WriterSubjects
	{ "CustomEntrySubject_11", 0x0000000Eu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_12", 0x0000000Fu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_13", 0x00000010u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_14", 0x00000011u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_15", 0x00000012u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_16", 0x00000013u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_17", 0x00000014u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_18", 0x00000015u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_19", 0x00000016u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_20", 0x00000017u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_21", 0x00000018u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_22", 0x00000019u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_23", 0x0000001Au, 0u },  // WriterSubjects
	{ "CustomEntrySubject_24", 0x0000001Bu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_25", 0x0000001Cu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_26", 0x0000001Du, 0u },  // WriterSubjects
	{ "CustomEntrySubject_27", 0x0000001Eu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_28", 0x0000001Fu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_29", 0x00000020u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_30", 0x00000021u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_31", 0x00000022u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_32", 0x00000023u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_33", 0x00000024u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_34", 0x00000025u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_35", 0x00000026u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_36", 0x00000027u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_37", 0x00000028u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_38", 0x00000029u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_39", 0x0000002Au, 0u },  // WriterSubjects
	{ "CustomEntrySubject_40", 0x0000002Bu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_41", 0x0000002Cu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_42", 0x0000002Du, 0u },  // WriterSubjects
	{ "CustomEntrySubject_43", 0x0000002Eu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_44", 0x0000002Fu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_45", 0x00000030u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_46", 0x00000031u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_47", 0x00000032u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_48", 0x00000033u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_49", 0x00000034u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_50", 0x00000035u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_51", 0x00000036u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_52", 0x00000037u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_53", 0x00000038u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_54", 0x00000039u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_55", 0x0000003Au, 0u },  // WriterSubjects
	{ "CustomEntrySubject_56", 0x0000003Bu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_57", 0x0000003Cu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_58", 0x0000003Du, 0u },  // WriterSubjects
	{ "CustomEntrySubject_59", 0x0000003Eu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_60", 0x0000003Fu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_61", 0x00000040u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_62", 0x00000041u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_63", 0x00000042u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_64", 0x00000043u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_65", 0x00000044u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_66", 0x00000045u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_67", 0x00000046u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_68", 0x00000047u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_69", 0x00000048u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_70", 0x00000049u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_71", 0x0000004Au, 0u },  // WriterSubjects
	{ "CustomEntrySubject_72", 0x0000004Bu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_73", 0x0000004Cu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_74", 0x0000004Du, 0u },  // WriterSubjects
	{ "CustomEntrySubject_75", 0x0000004Eu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_76", 0x0000004Fu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_77", 0x00000050u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_78", 0x00000051u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_79", 0x00000052u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_80", 0x00000053u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_81", 0x00000054u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_82", 0x00000055u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_83", 0x00000056u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_84", 0x00000057u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_85", 0x00000058u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_86", 0x00000059u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_87", 0x0000005Au, 0u },  // WriterSubjects
	{ "CustomEntrySubject_88", 0x0000005Bu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_89", 0x0000005Cu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_90", 0x0000005Du, 0u },  // WriterSubjects
	{ "CustomEntrySubject_91", 0x0000005Eu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_92", 0x0000005Fu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_93", 0x00000060u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_94", 0x00000061u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_95", 0x00000062u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_96", 0x00000063u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_97", 0x00000064u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_98", 0x00000065u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_99", 0x00000066u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_100", 0x00000067u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_101", 0x00000068u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_102", 0x00000069u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_103", 0x0000006Au, 0u },  // WriterSubjects
	{ "CustomEntrySubject_104", 0x0000006Bu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_105", 0x0000006Cu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_106", 0x0000006Du, 0u },  // WriterSubjects
	{ "CustomEntrySubject_107", 0x0000006Eu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_108", 0x0000006Fu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_109", 0x00000070u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_110", 0x00000071u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_111", 0x00000072u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_112", 0x00000073u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_113", 0x00000074u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_114", 0x00000075u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_115", 0x00000076u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_116", 0x00000077u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_117", 0x00000078u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_118", 0x00000079u, 0u },  // WriterSubjects
	{ "CustomEntrySubject_119", 0x0000007Au, 0u },  // WriterSubjects
	{ "CustomEntrySubject_120", 0x0000007Bu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_121", 0x0000007Cu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_122", 0x0000007Du, 0u },  // WriterSubjects
	{ "CustomEntrySubject_123", 0x0000007Eu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_124", 0x0000007Fu, 0u },  // WriterSubjects
	{ "CustomEntrySubject_125", 0x00000080u, 0u },  // WriterSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "WriterSubjects", "", 0u, 126u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[126] = {
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
	{ 0x00000014u, 17u },
	{ 0x00000015u, 18u },
	{ 0x00000016u, 19u },
	{ 0x00000017u, 20u },
	{ 0x00000018u, 21u },
	{ 0x00000019u, 22u },
	{ 0x0000001Au, 23u },
	{ 0x0000001Bu, 24u },
	{ 0x0000001Cu, 25u },
	{ 0x0000001Du, 26u },
	{ 0x0000001Eu, 27u },
	{ 0x0000001Fu, 28u },
	{ 0x00000020u, 29u },
	{ 0x00000021u, 30u },
	{ 0x00000022u, 31u },
	{ 0x00000023u, 32u },
	{ 0x00000024u, 33u },
	{ 0x00000025u, 34u },
	{ 0x00000026u, 35u },
	{ 0x00000027u, 36u },
	{ 0x00000028u, 37u },
	{ 0x00000029u, 38u },
	{ 0x0000002Au, 39u },
	{ 0x0000002Bu, 40u },
	{ 0x0000002Cu, 41u },
	{ 0x0000002Du, 42u },
	{ 0x0000002Eu, 43u },
	{ 0x0000002Fu, 44u },
	{ 0x00000030u, 45u },
	{ 0x00000031u, 46u },
	{ 0x00000032u, 47u },
	{ 0x00000033u, 48u },
	{ 0x00000034u, 49u },
	{ 0x00000035u, 50u },
	{ 0x00000036u, 51u },
	{ 0x00000037u, 52u },
	{ 0x00000038u, 53u },
	{ 0x00000039u, 54u },
	{ 0x0000003Au, 55u },
	{ 0x0000003Bu, 56u },
	{ 0x0000003Cu, 57u },
	{ 0x0000003Du, 58u },
	{ 0x0000003Eu, 59u },
	{ 0x0000003Fu, 60u },
	{ 0x00000040u, 61u },
	{ 0x00000041u, 62u },
	{ 0x00000042u, 63u },
	{ 0x00000043u, 64u },
	{ 0x00000044u, 65u },
	{ 0x00000045u, 66u },
	{ 0x00000046u, 67u },
	{ 0x00000047u, 68u },
	{ 0x00000048u, 69u },
	{ 0x00000049u, 70u },
	{ 0x0000004Au, 71u },
	{ 0x0000004Bu, 72u },
	{ 0x0000004Cu, 73u },
	{ 0x0000004Du, 74u },
	{ 0x0000004Eu, 75u },
	{ 0x0000004Fu, 76u },
	{ 0x00000050u, 77u },
	{ 0x00000051u, 78u },
	{ 0x00000052u, 79u },
	{ 0x00000053u, 80u },
	{ 0x00000054u, 81u },
	{ 0x00000055u, 82u },
	{ 0x00000056u, 83u },
	{ 0x00000057u, 84u },
	{ 0x00000058u, 85u },
	{ 0x00000059u, 86u },
	{ 0x0000005Au, 87u },
	{ 0x0000005Bu, 88u },
	{ 0x0000005Cu, 89u },
	{ 0x0000005Du, 90u },
	{ 0x0000005Eu, 91u },
	{ 0x0000005Fu, 92u },
	{ 0x00000060u, 93u },
	{ 0x00000061u, 94u },
	{ 0x00000062u, 95u },
	{ 0x00000063u, 96u },
	{ 0x00000064u, 97u },
	{ 0x00000065u, 98u },
	{ 0x00000066u, 99u },
	{ 0x00000067u, 100u },
	{ 0x00000068u, 101u },
	{ 0x00000069u, 102u },
	{ 0x0000006Au, 103u },
	{ 0x0000006Bu, 104u },
	{ 0x0000006Cu, 105u },
	{ 0x0000006Du, 106u },
	{ 0x0000006Eu, 107u },
	{ 0x0000006Fu, 108u },
	{ 0x00000070u, 109u },
	{ 0x00000071u, 110u },
	{ 0x00000072u, 111u },
	{ 0x00000073u, 112u },
	{ 0x00000074u, 113u },
	{ 0x00000075u, 114u },
	{ 0x00000076u, 115u },
	{ 0x00000077u, 116u },
	{ 0x00000078u, 117u },
	{ 0x00000079u, 118u },
	{ 0x0000007Au, 119u },
	{ 0x0000007Bu, 120u },
	{ 0x0000007Cu, 121u },
	{ 0x0000007Du, 122u },
	{ 0x0000007Eu, 123u },
	{ 0x0000007Fu, 124u },
	{ 0x00000080u, 125u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[126] = {
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_2
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_3
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_5
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_7
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_8
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_9
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_10
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_11
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_12
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_13
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_14
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_15
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_16
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_17
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_18
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_19), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_19
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_20), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_20
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_21), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_21
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_22), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_22
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_23), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_23
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_24), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_24
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_25), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_25
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_26), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_26
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_27), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_27
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_28), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_28
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_29), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_29
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_30), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_30
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_31), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_31
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_32
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_33), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_33
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_34), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_34
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_35), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_35
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_36), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_36
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_37), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_37
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_38), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_38
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_39), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_39
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_40), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_40
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_41), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_41
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_42), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_42
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_43), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_43
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_44), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_44
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_45), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_45
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_46), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_46
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_47), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_47
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_48), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_48
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_49), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_49
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_50), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_50
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_51), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_51
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_52), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_52
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_53), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_53
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_54), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_54
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_55), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_55
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_56), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_56
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_57), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_57
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_58), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_58
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_59), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_59
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_60), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_60
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_61), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_61
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_62), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_62
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_63), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_63
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_64), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_64
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_65), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_65
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_66), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_66
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_67), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_67
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_68), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_68
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_69), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_69
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_70), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_70
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_71), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_71
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_72), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_72
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_73), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_73
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_74), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_74
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_75), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_75
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_76), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_76
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_77), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_77
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_78), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_78
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_79), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_79
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_80), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_80
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_81), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_81
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_82), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_82
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_83), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_83
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_84), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_84
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_85), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_85
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_86), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_86
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_87), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_87
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_88), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_88
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_89), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_89
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_90), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_90
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_91), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_91
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_92), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_92
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_93), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_93
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_94), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_94
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_95), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_95
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_96), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_96
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_97), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_97
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_98), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_98
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_99), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_99
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_100), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_100
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_101), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_101
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_102), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_102
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_103), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_103
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_104), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_104
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_105), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_105
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_106), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_106
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_107), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_107
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_108), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_108
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_109), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_109
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_110), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_110
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_111), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_111
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_112), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_112
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_113), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_113
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_114), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_114
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_115), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_115
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_116), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_116
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_117), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_117
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_118), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_118
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_119), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_119
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_120), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_120
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_121), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_121
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_122), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_122
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_123), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_123
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_124), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_124
	{ reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_125), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchActive },  // WriterSubjects::CustomEntrySubject_125
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"WriterSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	126u,
	s_hotpatch_slots,
	126u,
	s_hotpatch_entries,
	126u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table (empty) ─────────────────
extern "C" const char* kChaosExternalRuntimeSubjects[1] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[1] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

// (no method AOT entries for this module)
// ── Dispatch table (kAotMethods[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
static void (*kAotMethods[126])() = {
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_0),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_1),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_2),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_3),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_4),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_5),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_6),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_7),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_8),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_9),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_10),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_11),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_12),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_13),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_14),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_15),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_16),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_17),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_18),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_19),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_20),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_21),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_22),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_23),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_24),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_25),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_26),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_27),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_28),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_29),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_30),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_31),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_32),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_33),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_34),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_35),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_36),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_37),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_38),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_39),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_40),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_41),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_42),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_43),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_44),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_45),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_46),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_47),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_48),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_49),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_50),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_51),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_52),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_53),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_54),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_55),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_56),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_57),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_58),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_59),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_60),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_61),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_62),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_63),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_64),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_65),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_66),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_67),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_68),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_69),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_70),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_71),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_72),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_73),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_74),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_75),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_76),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_77),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_78),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_79),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_80),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_81),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_82),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_83),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_84),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_85),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_86),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_87),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_88),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_89),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_90),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_91),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_92),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_93),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_94),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_95),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_96),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_97),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_98),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_99),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_100),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_101),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_102),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_103),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_104),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_105),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_106),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_107),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_108),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_109),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_110),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_111),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_112),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_113),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_114),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_115),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_116),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_117),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_118),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_119),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_120),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_121),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_122),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_123),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_124),
	reinterpret_cast<void(*)()>(&WriterSubjects_WriterSubjects_CustomEntrySubject_125),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[126])() = {
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
	[]() {kAotMethods[48]();},
	[]() {kAotMethods[49]();},
	[]() {kAotMethods[50]();},
	[]() {kAotMethods[51]();},
	[]() {kAotMethods[52]();},
	[]() {kAotMethods[53]();},
	[]() {kAotMethods[54]();},
	[]() {kAotMethods[55]();},
	[]() {kAotMethods[56]();},
	[]() {kAotMethods[57]();},
	[]() {kAotMethods[58]();},
	[]() {kAotMethods[59]();},
	[]() {kAotMethods[60]();},
	[]() {kAotMethods[61]();},
	[]() {kAotMethods[62]();},
	[]() {kAotMethods[63]();},
	[]() {kAotMethods[64]();},
	[]() {kAotMethods[65]();},
	[]() {kAotMethods[66]();},
	[]() {kAotMethods[67]();},
	[]() {kAotMethods[68]();},
	[]() {kAotMethods[69]();},
	[]() {kAotMethods[70]();},
	[]() {kAotMethods[71]();},
	[]() {kAotMethods[72]();},
	[]() {kAotMethods[73]();},
	[]() {kAotMethods[74]();},
	[]() {kAotMethods[75]();},
	[]() {kAotMethods[76]();},
	[]() {kAotMethods[77]();},
	[]() {kAotMethods[78]();},
	[]() {kAotMethods[79]();},
	[]() {kAotMethods[80]();},
	[]() {kAotMethods[81]();},
	[]() {kAotMethods[82]();},
	[]() {kAotMethods[83]();},
	[]() {kAotMethods[84]();},
	[]() {kAotMethods[85]();},
	[]() {kAotMethods[86]();},
	[]() {kAotMethods[87]();},
	[]() {kAotMethods[88]();},
	[]() {kAotMethods[89]();},
	[]() {kAotMethods[90]();},
	[]() {kAotMethods[91]();},
	[]() {kAotMethods[92]();},
	[]() {kAotMethods[93]();},
	[]() {kAotMethods[94]();},
	[]() {kAotMethods[95]();},
	[]() {kAotMethods[96]();},
	[]() {kAotMethods[97]();},
	[]() {kAotMethods[98]();},
	[]() {kAotMethods[99]();},
	[]() {kAotMethods[100]();},
	[]() {kAotMethods[101]();},
	[]() {kAotMethods[102]();},
	[]() {kAotMethods[103]();},
	[]() {kAotMethods[104]();},
	[]() {kAotMethods[105]();},
	[]() {kAotMethods[106]();},
	[]() {kAotMethods[107]();},
	[]() {kAotMethods[108]();},
	[]() {kAotMethods[109]();},
	[]() {kAotMethods[110]();},
	[]() {kAotMethods[111]();},
	[]() {kAotMethods[112]();},
	[]() {kAotMethods[113]();},
	[]() {kAotMethods[114]();},
	[]() {kAotMethods[115]();},
	[]() {kAotMethods[116]();},
	[]() {kAotMethods[117]();},
	[]() {kAotMethods[118]();},
	[]() {kAotMethods[119]();},
	[]() {kAotMethods[120]();},
	[]() {kAotMethods[121]();},
	[]() {kAotMethods[122]();},
	[]() {kAotMethods[123]();},
	[]() {kAotMethods[124]();},
	[]() {kAotMethods[125]();},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 126;
extern "C" const int kSubjectEntryIndices[126] = {
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
	48,
	49,
	50,
	51,
	52,
	53,
	54,
	55,
	56,
	57,
	58,
	59,
	60,
	61,
	62,
	63,
	64,
	65,
	66,
	67,
	68,
	69,
	70,
	71,
	72,
	73,
	74,
	75,
	76,
	77,
	78,
	79,
	80,
	81,
	82,
	83,
	84,
	85,
	86,
	87,
	88,
	89,
	90,
	91,
	92,
	93,
	94,
	95,
	96,
	97,
	98,
	99,
	100,
	101,
	102,
	103,
	104,
	105,
	106,
	107,
	108,
	109,
	110,
	111,
	112,
	113,
	114,
	115,
	116,
	117,
	118,
	119,
	120,
	121,
	122,
	123,
	124,
	125,
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
// ── GC Slot Map Section (empty) ───────────────────────────────
static const CHAOS_IL2CPP_UINT8 kChaosGcSlotMapsSection[1] = { 0 };
static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 0;

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[126] = {
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_0),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_1),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_2),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_3),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_4),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_5),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_6),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_7),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_8),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_9),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_10),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_11),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_12),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_13),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_14),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_15),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_16),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_17),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_18),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_19),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_20),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_21),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_22),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_23),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_24),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_25),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_26),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_27),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_28),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_29),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_30),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_31),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_32),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_33),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_34),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_35),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_36),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_37),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_38),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_39),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_40),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_41),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_42),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_43),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_44),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_45),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_46),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_47),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_48),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_49),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_50),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_51),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_52),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_53),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_54),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_55),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_56),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_57),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_58),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_59),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_60),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_61),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_62),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_63),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_64),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_65),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_66),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_67),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_68),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_69),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_70),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_71),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_72),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_73),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_74),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_75),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_76),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_77),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_78),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_79),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_80),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_81),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_82),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_83),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_84),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_85),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_86),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_87),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_88),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_89),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_90),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_91),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_92),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_93),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_94),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_95),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_96),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_97),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_98),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_99),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_100),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_101),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_102),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_103),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_104),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_105),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_106),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_107),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_108),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_109),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_110),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_111),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_112),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_113),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_114),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_115),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_116),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_117),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_118),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_119),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_120),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_121),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_122),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_123),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_124),
	reinterpret_cast<void*>(&WriterSubjects_WriterSubjects_CustomEntrySubject_125),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 126u,
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
	.slot_map_section_begin = nullptr,
	.slot_map_section_end   = nullptr,
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
	.image_name_utf8    = "WriterSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_WriterSubjects_WriterSubjects[1] = {
	{ 0u, "WriterSubjects/WriterSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_WriterSubjects_WriterSubjects[126] = {
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_2:System.Void()", "CustomEntrySubject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_3:System.Void()", "CustomEntrySubject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_5:System.Void()", "CustomEntrySubject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_9:System.Void()", "CustomEntrySubject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_10:System.Void()", "CustomEntrySubject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_11:System.Void()", "CustomEntrySubject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_12:System.Void()", "CustomEntrySubject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_13:System.Void()", "CustomEntrySubject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_14:System.Void()", "CustomEntrySubject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_15:System.Void()", "CustomEntrySubject_15", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_16:System.Void()", "CustomEntrySubject_16", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_17:System.Void()", "CustomEntrySubject_17", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_18:System.Void()", "CustomEntrySubject_18", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_19:System.Void()", "CustomEntrySubject_19", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_20:System.Void()", "CustomEntrySubject_20", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_21:System.Void()", "CustomEntrySubject_21", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_22:System.Void()", "CustomEntrySubject_22", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_23:System.Void()", "CustomEntrySubject_23", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_24:System.Void()", "CustomEntrySubject_24", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_25:System.Void()", "CustomEntrySubject_25", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_26:System.Void()", "CustomEntrySubject_26", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_27:System.Void()", "CustomEntrySubject_27", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_28:System.Void()", "CustomEntrySubject_28", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_29:System.Void()", "CustomEntrySubject_29", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_30:System.Void()", "CustomEntrySubject_30", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_31:System.Void()", "CustomEntrySubject_31", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_32:System.Void()", "CustomEntrySubject_32", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_33:System.Void()", "CustomEntrySubject_33", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_34:System.Void()", "CustomEntrySubject_34", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_35:System.Void()", "CustomEntrySubject_35", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_36:System.Void()", "CustomEntrySubject_36", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_37:System.Void()", "CustomEntrySubject_37", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_38:System.Void()", "CustomEntrySubject_38", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_39:System.Void()", "CustomEntrySubject_39", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_40:System.Void()", "CustomEntrySubject_40", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_41:System.Void()", "CustomEntrySubject_41", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_42:System.Void()", "CustomEntrySubject_42", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_43:System.Void()", "CustomEntrySubject_43", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_44:System.Void()", "CustomEntrySubject_44", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_45:System.Void()", "CustomEntrySubject_45", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_46:System.Void()", "CustomEntrySubject_46", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_47:System.Void()", "CustomEntrySubject_47", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_48:System.Void()", "CustomEntrySubject_48", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_49:System.Void()", "CustomEntrySubject_49", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_50:System.Void()", "CustomEntrySubject_50", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_51:System.Void()", "CustomEntrySubject_51", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_52:System.Void()", "CustomEntrySubject_52", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_53:System.Void()", "CustomEntrySubject_53", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_54:System.Void()", "CustomEntrySubject_54", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_55:System.Void()", "CustomEntrySubject_55", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_56:System.Void()", "CustomEntrySubject_56", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_57:System.Void()", "CustomEntrySubject_57", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_58:System.Void()", "CustomEntrySubject_58", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_59:System.Void()", "CustomEntrySubject_59", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_60:System.Void()", "CustomEntrySubject_60", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_61:System.Void()", "CustomEntrySubject_61", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_62:System.Void()", "CustomEntrySubject_62", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_63:System.Void()", "CustomEntrySubject_63", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_64:System.Void()", "CustomEntrySubject_64", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_65:System.Void()", "CustomEntrySubject_65", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_66:System.Void()", "CustomEntrySubject_66", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_67:System.Void()", "CustomEntrySubject_67", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_68:System.Void()", "CustomEntrySubject_68", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_69:System.Void()", "CustomEntrySubject_69", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_70:System.Void()", "CustomEntrySubject_70", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_71:System.Void()", "CustomEntrySubject_71", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_72:System.Void()", "CustomEntrySubject_72", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_73:System.Void()", "CustomEntrySubject_73", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_74:System.Void()", "CustomEntrySubject_74", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_75:System.Void()", "CustomEntrySubject_75", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_76:System.Void()", "CustomEntrySubject_76", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_77:System.Void()", "CustomEntrySubject_77", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_78:System.Void()", "CustomEntrySubject_78", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_79:System.Void()", "CustomEntrySubject_79", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_80:System.Void()", "CustomEntrySubject_80", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_81:System.Void()", "CustomEntrySubject_81", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_82:System.Void()", "CustomEntrySubject_82", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_83:System.Void()", "CustomEntrySubject_83", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_84:System.Void()", "CustomEntrySubject_84", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_85:System.Void()", "CustomEntrySubject_85", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_86:System.Void()", "CustomEntrySubject_86", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_87:System.Void()", "CustomEntrySubject_87", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_88:System.Void()", "CustomEntrySubject_88", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_89:System.Void()", "CustomEntrySubject_89", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_90:System.Void()", "CustomEntrySubject_90", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_91:System.Void()", "CustomEntrySubject_91", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_92:System.Void()", "CustomEntrySubject_92", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_93:System.Void()", "CustomEntrySubject_93", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_94:System.Void()", "CustomEntrySubject_94", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_95:System.Void()", "CustomEntrySubject_95", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_96:System.Void()", "CustomEntrySubject_96", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_97:System.Void()", "CustomEntrySubject_97", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_98:System.Void()", "CustomEntrySubject_98", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_99:System.Void()", "CustomEntrySubject_99", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_100:System.Void()", "CustomEntrySubject_100", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_101:System.Void()", "CustomEntrySubject_101", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_102:System.Void()", "CustomEntrySubject_102", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_103:System.Void()", "CustomEntrySubject_103", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_104:System.Void()", "CustomEntrySubject_104", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_105:System.Void()", "CustomEntrySubject_105", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_106:System.Void()", "CustomEntrySubject_106", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_107:System.Void()", "CustomEntrySubject_107", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_108:System.Void()", "CustomEntrySubject_108", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_109:System.Void()", "CustomEntrySubject_109", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_110:System.Void()", "CustomEntrySubject_110", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_111:System.Void()", "CustomEntrySubject_111", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_112:System.Void()", "CustomEntrySubject_112", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_113:System.Void()", "CustomEntrySubject_113", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_114:System.Void()", "CustomEntrySubject_114", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_115:System.Void()", "CustomEntrySubject_115", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_116:System.Void()", "CustomEntrySubject_116", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_117:System.Void()", "CustomEntrySubject_117", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_118:System.Void()", "CustomEntrySubject_118", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_119:System.Void()", "CustomEntrySubject_119", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_120:System.Void()", "CustomEntrySubject_120", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_121:System.Void()", "CustomEntrySubject_121", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_122:System.Void()", "CustomEntrySubject_122", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_123:System.Void()", "CustomEntrySubject_123", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_124:System.Void()", "CustomEntrySubject_124", "System.Void", 0, nullptr, 0u },
	{ 0u, "WriterSubjects/WriterSubjects::CustomEntrySubject_125:System.Void()", "CustomEntrySubject_125", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "WriterSubjects/WriterSubjects", "WriterSubjects/WriterSubjects", "", "WriterSubjects", "WriterSubjects", nullptr, kReflFields_WriterSubjects_WriterSubjects, 1u, nullptr, 0u,
	kReflMethods_WriterSubjects_WriterSubjects, 126u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "WriterSubjects", kReflTypePtrs, 1u };

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
// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_0()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_1()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_2()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_3()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_4()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_5()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_6()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_7()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_8()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_9()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_10()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_11()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_12()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_13()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_14()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_14(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_15()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_15(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_16()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_16(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_17()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_17(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_18()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_18(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_19()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_19(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_20()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_20(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_21()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_21(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_22()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_22(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_23()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_23(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_24()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_24(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_25()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_25(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_26()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_26(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_27()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_27(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_28()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_28(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_29()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_29(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_30()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_30(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_31()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_31(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_32()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_32(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_33()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_33(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_34()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_34(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_35()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_35(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_36()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_36(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_37()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_37(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_38()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_38(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_39()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_39(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_40()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_40(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_41()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_41(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_42()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_42(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_43()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_43(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_44()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_44(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_45()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_45(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_46()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_46(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_47()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_47(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_48()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_48(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_49()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_49(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_50()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_50(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_51()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_51(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_52()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_52(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_53()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_53(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_54()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_54(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_55()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_55(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_56()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_56(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_57()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_57(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_58()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_58(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_59()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_59(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_60()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_60(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_61()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_61(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_62()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_62(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_63()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_63(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_64()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_64(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_65()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_65(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_66()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_66(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_67()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_67(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_68()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_68(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_69()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_69(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_70()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_70(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_71()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_71(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_72()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_72(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_73()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_73(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_74()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_74(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_75()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_75(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_76()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_76(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_77()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_77(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_78()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_78(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_79()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_79(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_80()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_80(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_81()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_81(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_82()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_82(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_83()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_83(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_84()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_84(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_85()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_85(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_86()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_86(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_87()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_87(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_88()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_88(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_89()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_89(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_90()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_90(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_91()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_91(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_92()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_92(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_93()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_93(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_94()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_94(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_95()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_95(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_96()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_96(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_97()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_97(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_98()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_98(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_99()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_99(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_100()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_100(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_101()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_101(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_102()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_102(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_103()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_103(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_104()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_104(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_105()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_105(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_106()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_106(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_107()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_107(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_108()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_108(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_109()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_109(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_110()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_110(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_111()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_111(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_112()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_112(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_113()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_113(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_114()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_114(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_115()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_115(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_116()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_116(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_117()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_117(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_118()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_118(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_119()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_119(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_120()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_120(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_121()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_121(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_122()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_122(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_123()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_123(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_124()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_124(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: WriterSubjects/WriterSubjects::CustomEntrySubject_125()
extern "C" void WriterSubjects_WriterSubjects_CustomEntrySubject_125(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}



}  // namespace chaos::il2cpp::codegen::WriterSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 126;

// ── JIT Method Entry Table ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kMethodJson_0[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-0\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_0:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_0()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_0:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_0:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-0\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_0()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_1[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-1\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_1:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_1()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_1:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_1:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-1\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_1()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_2[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-2\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_2:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_2()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_2:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_2:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-2\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_2()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_3[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-3\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_3:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_3()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_3:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_3:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-3\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_3()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_4[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-4\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_4:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_4()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_4:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_4:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-4\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_4()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_5[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-5\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_5:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_5()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_5:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_5:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-5\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_5()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_6[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-6\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_6:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_6()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_6:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_6:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-6\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_6()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_7[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-7\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_7:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_7()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_7:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_7:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-7\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_7()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_8[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-8\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_8:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_8()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_8:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_8:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-8\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_8()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_9[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-9\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_9:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_9()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_9:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_9:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-9\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_9()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_9\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_10[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-10\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_10:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_10()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_10:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_10:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-10\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_10()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_10\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_11[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-11\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_11:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_11()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_11:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_11:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-11\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_11()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_11\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_12[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-12\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_12:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_12()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_12:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_12:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-12\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_12()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_12\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_13[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-13\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_13:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_13()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_13:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_13:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-13\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_13()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_13\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_14[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-14\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_14:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_14()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_14:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_14:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-14\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_14()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_14\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_15[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-15\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_15:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_15()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_15:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_15:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-15\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_15()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_15\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_16[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-16\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_16:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_16()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_16:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_16:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-16\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_16()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_16\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_17[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-17\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_17:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_17()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_17:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_17:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-17\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_17()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_17\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_18[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-18\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_18:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_18()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_18:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_18:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-18\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_18()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_18\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_19[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-19\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_19:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_19()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_19:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_19:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-19\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_19()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_19\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_20[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-20\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_20:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_20()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_20:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_20:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-20\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_20()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_20\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_21[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-21\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_21:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_21()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_21:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_21:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-21\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_21()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_21\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_22[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-22\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_22:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_22()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_22:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_22:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-22\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_22()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_22\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_23[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-23\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_23:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_23()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_23:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_23:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-23\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_23()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_23\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_24[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-24\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_24:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_24()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_24:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_24:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-24\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_24()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_24\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_25[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-25\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_25:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_25()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_25:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_25:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-25\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_25()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_25\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_26[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-26\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_26:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_26()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_26:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_26:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-26\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_26()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_26\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_27[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-27\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_27:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_27()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_27:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_27:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-27\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_27()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_27\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_28[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-28\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_28:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_28()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_28:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_28:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-28\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_28()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_28\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_29[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-29\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_29:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_29()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_29:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_29:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-29\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_29()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_29\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_30[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-30\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_30:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_30()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_30:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_30:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-30\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_30()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_30\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_31[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-31\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_31:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_31()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_31:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_31:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-31\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_31()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_31\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_32[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-32\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_32:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_32()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_32:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_32:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-32\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_32()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_32\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_33[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-33\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_33:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_33()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_33:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_33:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-33\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_33()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_33\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_34[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-34\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_34:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_34()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_34:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_34:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-34\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_34()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_34\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_35[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-35\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_35:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_35()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_35:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_35:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-35\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_35()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_35\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_36[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-36\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_36:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_36()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_36:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_36:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-36\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_36()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_36\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_37[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-37\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_37:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_37()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_37:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_37:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-37\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_37()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_37\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_38[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-38\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_38:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_38()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_38:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_38:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-38\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_38()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_38\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_39[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-39\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_39:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_39()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_39:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_39:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-39\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_39()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_39\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_40[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-40\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_40:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_40()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_40:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_40:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-40\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_40()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_40\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_41[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-41\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_41:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_41()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_41:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_41:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-41\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_41()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_41\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_42[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-42\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_42:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_42()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_42:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_42:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-42\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_42()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_42\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_43[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-43\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_43:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_43()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_43:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_43:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-43\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_43()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_43\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_44[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-44\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_44:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_44()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_44:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_44:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-44\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_44()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_44\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_45[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-45\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_45:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_45()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_45:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_45:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-45\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_45()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_45\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_46[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-46\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_46:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_46()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_46:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_46:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-46\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_46()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_46\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_47[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-47\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_47:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_47()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_47:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_47:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-47\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_47()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_47\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_48[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-48\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_48:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_48()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_48:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_48:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-48\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_48()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_48\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_49[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-49\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_49:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_49()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_49:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_49:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-49\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_49()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_49\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_50[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-50\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_50:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_50()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_50:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_50:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-50\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_50()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_50\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_51[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-51\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_51:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_51()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_51:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_51:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-51\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_51()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_51\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_52[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-52\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_52:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_52()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_52:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_52:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-52\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_52()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_52\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_53[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-53\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_53:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_53()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_53:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_53:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-53\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_53()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_53\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_54[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-54\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_54:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_54()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_54:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_54:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-54\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_54()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_54\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_55[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-55\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_55:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_55()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_55:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_55:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-55\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_55()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_55\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_56[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-56\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_56:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_56()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_56:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_56:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-56\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_56()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_56\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_57[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-57\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_57:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_57()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_57:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_57:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-57\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_57()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_57\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_58[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-58\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_58:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_58()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_58:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_58:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-58\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_58()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_58\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_59[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-59\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_59:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_59()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_59:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_59:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-59\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_59()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_59\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_60[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-60\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_60:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_60()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_60:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_60:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-60\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_60()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_60\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_61[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-61\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_61:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_61()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_61:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_61:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-61\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_61()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_61\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_62[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-62\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_62:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_62()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_62:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_62:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-62\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_62()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_62\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_63[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-63\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_63:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_63()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_63:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_63:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-63\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_63()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_63\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_64[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-64\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_64:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_64()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_64:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_64:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-64\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_64()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_64\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_65[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-65\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_65:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_65()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_65:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_65:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-65\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_65()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_65\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_66[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-66\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_66:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_66()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_66:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_66:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-66\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_66()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_66\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_67[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-67\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_67:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_67()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_67:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_67:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-67\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_67()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_67\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_68[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-68\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_68:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_68()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_68:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_68:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-68\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_68()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_68\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_69[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-69\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_69:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_69()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_69:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_69:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-69\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_69()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_69\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_70[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-70\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_70:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_70()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_70:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_70:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-70\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_70()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_70\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_71[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-71\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_71:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_71()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_71:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_71:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-71\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_71()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_71\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_72[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-72\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_72:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_72()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_72:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_72:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-72\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_72()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_72\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_73[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-73\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_73:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_73()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_73:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_73:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-73\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_73()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_73\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_74[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-74\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_74:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_74()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_74:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_74:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-74\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_74()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_74\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_75[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-75\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_75:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_75()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_75:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_75:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-75\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_75()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_75\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_76[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-76\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_76:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_76()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_76:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_76:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-76\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_76()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_76\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_77[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-77\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_77:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_77()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_77:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_77:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-77\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_77()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_77\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_78[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-78\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_78:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_78()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_78:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_78:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-78\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_78()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_78\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_79[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-79\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_79:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_79()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_79:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_79:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-79\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_79()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_79\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_80[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-80\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_80:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_80()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_80:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_80:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-80\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_80()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_80\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_81[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-81\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_81:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_81()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_81:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_81:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-81\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_81()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_81\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_82[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-82\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_82:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_82()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_82:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_82:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-82\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_82()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_82\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_83[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-83\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_83:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_83()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_83:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_83:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-83\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_83()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_83\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_84[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-84\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_84:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_84()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_84:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_84:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-84\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_84()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_84\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_85[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-85\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_85:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_85()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_85:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_85:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-85\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_85()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_85\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_86[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-86\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_86:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_86()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_86:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_86:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-86\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_86()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_86\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_87[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-87\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_87:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_87()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_87:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_87:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-87\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_87()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_87\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_88[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-88\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_88:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_88()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_88:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_88:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-88\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_88()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_88\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_89[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-89\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_89:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_89()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_89:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_89:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-89\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_89()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_89\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_90[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-90\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_90:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_90()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_90:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_90:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-90\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_90()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_90\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_91[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-91\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_91:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_91()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_91:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_91:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-91\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_91()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_91\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_92[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-92\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_92:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_92()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_92:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_92:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-92\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_92()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_92\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_93[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-93\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_93:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_93()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_93:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_93:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-93\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_93()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_93\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_94[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-94\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_94:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_94()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_94:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_94:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-94\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_94()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_94\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_95[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-95\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_95:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_95()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_95:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_95:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-95\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_95()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_95\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_96[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-96\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_96:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_96()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_96:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_96:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-96\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_96()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_96\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_97[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-97\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_97:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_97()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_97:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_97:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-97\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_97()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_97\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_98[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-98\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_98:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_98()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_98:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_98:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-98\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_98()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_98\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_99[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-99\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_99:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_99()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_99:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_99:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-99\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_99()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_99\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_100[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-100\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_100:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_100()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_100:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_100:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-100\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_100()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_100\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_101[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-101\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_101:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_101()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_101:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_101:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-101\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_101()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_101\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_102[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-102\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_102:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_102()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_102:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_102:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-102\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_102()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_102\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_103[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-103\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_103:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_103()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_103:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_103:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-103\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_103()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_103\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_104[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-104\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_104:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_104()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_104:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_104:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-104\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_104()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_104\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_105[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-105\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_105:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_105()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_105:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_105:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-105\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_105()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_105\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_106[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-106\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_106:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_106()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_106:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_106:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-106\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_106()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_106\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_107[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-107\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_107:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_107()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_107:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_107:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-107\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_107()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_107\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_108[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-108\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_108:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_108()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_108:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_108:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-108\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_108()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_108\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_109[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-109\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_109:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_109()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_109:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_109:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-109\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_109()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_109\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_110[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-110\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_110:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_110()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_110:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_110:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-110\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_110()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_110\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_111[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-111\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_111:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_111()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_111:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_111:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-111\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_111()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_111\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_112[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-112\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_112:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_112()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_112:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_112:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-112\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_112()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_112\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_113[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-113\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_113:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_113()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_113:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_113:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-113\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_113()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_113\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_114[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-114\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_114:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_114()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_114:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_114:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-114\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_114()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_114\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_115[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-115\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_115:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_115()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_115:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_115:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-115\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_115()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_115\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_116[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-116\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_116:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_116()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_116:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_116:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-116\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_116()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_116\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_117[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-117\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_117:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_117()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_117:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_117:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-117\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_117()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_117\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_118[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-118\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_118:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_118()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_118:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_118:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-118\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_118()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_118\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_119[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-119\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_119:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_119()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_119:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_119:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-119\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_119()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_119\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_120[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-120\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_120:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_120()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_120:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_120:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-120\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_120()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_120\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_121[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-121\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_121:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_121()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_121:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_121:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-121\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_121()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_121\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_122[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-122\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_122:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_122()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_122:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_122:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-122\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_122()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_122\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_123[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-123\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_123:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_123()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_123:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_123:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-123\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_123()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_123\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_124[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-124\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_124:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_124()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_124:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_124:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-124\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_124()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_124\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";
static const char kMethodJson_125[] = "{\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-125\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_125:System.Void()\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_125()\",\"identity\":{\"assemblyName\":\"WriterSubjects\",\"declaringTypeSubjectId\":\"WriterSubjects/WriterSubjects\",\"definitionSubjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_125:System.Void()\",\"subjectId\":\"WriterSubjects/WriterSubjects::CustomEntrySubject_125:System.Void()\",\"methodId\":\"writer-subjects.writer-subjects.custom-entry-subject-125\",\"signature\":\"System.Void WriterSubjects::CustomEntrySubject_125()\"},\"nativeSymbol\":\"WriterSubjects_WriterSubjects_CustomEntrySubject_125\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":1,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"WriterSubjects\"}";

extern "C" const JitMethodEntry kChaosJitMethodEntries[126] =
{
    { kMethodJson_0, 1205u, 0x00000003u, 0u },
    { kMethodJson_1, 1205u, 0x00000004u, 0u },
    { kMethodJson_2, 1205u, 0x00000005u, 0u },
    { kMethodJson_3, 1205u, 0x00000006u, 0u },
    { kMethodJson_4, 1205u, 0x00000007u, 0u },
    { kMethodJson_5, 1205u, 0x00000008u, 0u },
    { kMethodJson_6, 1205u, 0x00000009u, 0u },
    { kMethodJson_7, 1205u, 0x0000000Au, 0u },
    { kMethodJson_8, 1205u, 0x0000000Bu, 0u },
    { kMethodJson_9, 1205u, 0x0000000Cu, 0u },
    { kMethodJson_10, 1213u, 0x0000000Du, 0u },
    { kMethodJson_11, 1213u, 0x0000000Eu, 0u },
    { kMethodJson_12, 1213u, 0x0000000Fu, 0u },
    { kMethodJson_13, 1213u, 0x00000010u, 0u },
    { kMethodJson_14, 1213u, 0x00000011u, 0u },
    { kMethodJson_15, 1213u, 0x00000012u, 0u },
    { kMethodJson_16, 1213u, 0x00000013u, 0u },
    { kMethodJson_17, 1213u, 0x00000014u, 0u },
    { kMethodJson_18, 1213u, 0x00000015u, 0u },
    { kMethodJson_19, 1213u, 0x00000016u, 0u },
    { kMethodJson_20, 1213u, 0x00000017u, 0u },
    { kMethodJson_21, 1213u, 0x00000018u, 0u },
    { kMethodJson_22, 1213u, 0x00000019u, 0u },
    { kMethodJson_23, 1213u, 0x0000001Au, 0u },
    { kMethodJson_24, 1213u, 0x0000001Bu, 0u },
    { kMethodJson_25, 1213u, 0x0000001Cu, 0u },
    { kMethodJson_26, 1213u, 0x0000001Du, 0u },
    { kMethodJson_27, 1213u, 0x0000001Eu, 0u },
    { kMethodJson_28, 1213u, 0x0000001Fu, 0u },
    { kMethodJson_29, 1213u, 0x00000020u, 0u },
    { kMethodJson_30, 1213u, 0x00000021u, 0u },
    { kMethodJson_31, 1213u, 0x00000022u, 0u },
    { kMethodJson_32, 1213u, 0x00000023u, 0u },
    { kMethodJson_33, 1213u, 0x00000024u, 0u },
    { kMethodJson_34, 1213u, 0x00000025u, 0u },
    { kMethodJson_35, 1213u, 0x00000026u, 0u },
    { kMethodJson_36, 1213u, 0x00000027u, 0u },
    { kMethodJson_37, 1213u, 0x00000028u, 0u },
    { kMethodJson_38, 1213u, 0x00000029u, 0u },
    { kMethodJson_39, 1213u, 0x0000002Au, 0u },
    { kMethodJson_40, 1213u, 0x0000002Bu, 0u },
    { kMethodJson_41, 1213u, 0x0000002Cu, 0u },
    { kMethodJson_42, 1213u, 0x0000002Du, 0u },
    { kMethodJson_43, 1213u, 0x0000002Eu, 0u },
    { kMethodJson_44, 1213u, 0x0000002Fu, 0u },
    { kMethodJson_45, 1213u, 0x00000030u, 0u },
    { kMethodJson_46, 1213u, 0x00000031u, 0u },
    { kMethodJson_47, 1213u, 0x00000032u, 0u },
    { kMethodJson_48, 1213u, 0x00000033u, 0u },
    { kMethodJson_49, 1213u, 0x00000034u, 0u },
    { kMethodJson_50, 1213u, 0x00000035u, 0u },
    { kMethodJson_51, 1213u, 0x00000036u, 0u },
    { kMethodJson_52, 1213u, 0x00000037u, 0u },
    { kMethodJson_53, 1213u, 0x00000038u, 0u },
    { kMethodJson_54, 1213u, 0x00000039u, 0u },
    { kMethodJson_55, 1213u, 0x0000003Au, 0u },
    { kMethodJson_56, 1213u, 0x0000003Bu, 0u },
    { kMethodJson_57, 1213u, 0x0000003Cu, 0u },
    { kMethodJson_58, 1213u, 0x0000003Du, 0u },
    { kMethodJson_59, 1213u, 0x0000003Eu, 0u },
    { kMethodJson_60, 1213u, 0x0000003Fu, 0u },
    { kMethodJson_61, 1213u, 0x00000040u, 0u },
    { kMethodJson_62, 1213u, 0x00000041u, 0u },
    { kMethodJson_63, 1213u, 0x00000042u, 0u },
    { kMethodJson_64, 1213u, 0x00000043u, 0u },
    { kMethodJson_65, 1213u, 0x00000044u, 0u },
    { kMethodJson_66, 1213u, 0x00000045u, 0u },
    { kMethodJson_67, 1213u, 0x00000046u, 0u },
    { kMethodJson_68, 1213u, 0x00000047u, 0u },
    { kMethodJson_69, 1213u, 0x00000048u, 0u },
    { kMethodJson_70, 1213u, 0x00000049u, 0u },
    { kMethodJson_71, 1213u, 0x0000004Au, 0u },
    { kMethodJson_72, 1213u, 0x0000004Bu, 0u },
    { kMethodJson_73, 1213u, 0x0000004Cu, 0u },
    { kMethodJson_74, 1213u, 0x0000004Du, 0u },
    { kMethodJson_75, 1213u, 0x0000004Eu, 0u },
    { kMethodJson_76, 1213u, 0x0000004Fu, 0u },
    { kMethodJson_77, 1213u, 0x00000050u, 0u },
    { kMethodJson_78, 1213u, 0x00000051u, 0u },
    { kMethodJson_79, 1213u, 0x00000052u, 0u },
    { kMethodJson_80, 1213u, 0x00000053u, 0u },
    { kMethodJson_81, 1213u, 0x00000054u, 0u },
    { kMethodJson_82, 1213u, 0x00000055u, 0u },
    { kMethodJson_83, 1213u, 0x00000056u, 0u },
    { kMethodJson_84, 1213u, 0x00000057u, 0u },
    { kMethodJson_85, 1213u, 0x00000058u, 0u },
    { kMethodJson_86, 1213u, 0x00000059u, 0u },
    { kMethodJson_87, 1213u, 0x0000005Au, 0u },
    { kMethodJson_88, 1213u, 0x0000005Bu, 0u },
    { kMethodJson_89, 1213u, 0x0000005Cu, 0u },
    { kMethodJson_90, 1213u, 0x0000005Du, 0u },
    { kMethodJson_91, 1213u, 0x0000005Eu, 0u },
    { kMethodJson_92, 1213u, 0x0000005Fu, 0u },
    { kMethodJson_93, 1213u, 0x00000060u, 0u },
    { kMethodJson_94, 1213u, 0x00000061u, 0u },
    { kMethodJson_95, 1213u, 0x00000062u, 0u },
    { kMethodJson_96, 1213u, 0x00000063u, 0u },
    { kMethodJson_97, 1213u, 0x00000064u, 0u },
    { kMethodJson_98, 1213u, 0x00000065u, 0u },
    { kMethodJson_99, 1213u, 0x00000066u, 0u },
    { kMethodJson_100, 1221u, 0x00000067u, 0u },
    { kMethodJson_101, 1221u, 0x00000068u, 0u },
    { kMethodJson_102, 1221u, 0x00000069u, 0u },
    { kMethodJson_103, 1221u, 0x0000006Au, 0u },
    { kMethodJson_104, 1221u, 0x0000006Bu, 0u },
    { kMethodJson_105, 1221u, 0x0000006Cu, 0u },
    { kMethodJson_106, 1221u, 0x0000006Du, 0u },
    { kMethodJson_107, 1221u, 0x0000006Eu, 0u },
    { kMethodJson_108, 1221u, 0x0000006Fu, 0u },
    { kMethodJson_109, 1221u, 0x00000070u, 0u },
    { kMethodJson_110, 1221u, 0x00000071u, 0u },
    { kMethodJson_111, 1221u, 0x00000072u, 0u },
    { kMethodJson_112, 1221u, 0x00000073u, 0u },
    { kMethodJson_113, 1221u, 0x00000074u, 0u },
    { kMethodJson_114, 1221u, 0x00000075u, 0u },
    { kMethodJson_115, 1221u, 0x00000076u, 0u },
    { kMethodJson_116, 1221u, 0x00000077u, 0u },
    { kMethodJson_117, 1221u, 0x00000078u, 0u },
    { kMethodJson_118, 1221u, 0x00000079u, 0u },
    { kMethodJson_119, 1221u, 0x0000007Au, 0u },
    { kMethodJson_120, 1221u, 0x0000007Bu, 0u },
    { kMethodJson_121, 1221u, 0x0000007Cu, 0u },
    { kMethodJson_122, 1221u, 0x0000007Du, 0u },
    { kMethodJson_123, 1221u, 0x0000007Eu, 0u },
    { kMethodJson_124, 1221u, 0x0000007Fu, 0u },
    { kMethodJson_125, 1221u, 0x00000080u, 0u }
};

extern "C" const uint32_t kChaosJitMethodEntryCount = 126u;

extern "C" void ChaosJitRegisterAll() {
    RegisterJitMethods(kChaosJitMethodEntries, kChaosJitMethodEntryCount);
}