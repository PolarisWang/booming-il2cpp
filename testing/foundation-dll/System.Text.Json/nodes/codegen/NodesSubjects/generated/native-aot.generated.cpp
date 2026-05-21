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

namespace chaos::il2cpp::codegen::NodesSubjects {

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

extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_0(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_1(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_2(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_3(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_4(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_5(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_6(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_7(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_8(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_9(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_10(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_11(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_12(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_13(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_14(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_15(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_16(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_17(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_18(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_19(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_20(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_21(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_22(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_23(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_24(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_25(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_26(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_27(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_28(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_29(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_30(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_31(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_32(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_33(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_34(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_35(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_36(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_37(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_38(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_39(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_40(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_41(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_42(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_43(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_44(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_45(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_46(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_47(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_48(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_49(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_50(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_51(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_52(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_53(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_54(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_55(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_56(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_57(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_58(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_59(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_60(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_61(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_62(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_63(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_64(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_65(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_66(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_67(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_68(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_69(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_70(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_71(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_72(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_73(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_74(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_75(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_76(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_77(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_78(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_79(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_80(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_81(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_82(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_83(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_84(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_85(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_86(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_87(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_88(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_89(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_90(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_91(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_92(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_93(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_94(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_95(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_96(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_97(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_98(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_99(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_100(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_101(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_102(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_103(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_104(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_105(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_106(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_107(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_108(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_109(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_110(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_111(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_112(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_113(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_114(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_115(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_116(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_117(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_118(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_119(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_120(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_121(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_122(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_123(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_124(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_125(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_126(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_127(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_128(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_129(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_130(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_131(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_132(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_133(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_134(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_135(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_136(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_137(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_138(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_139(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_140(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_141(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_142(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_143(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_144(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_145(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_146(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_147(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_148(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_149(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_150(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_151(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_152(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_153(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_154(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_155(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_156(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_157(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_158(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_159(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_160(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_161(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_162(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_163(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_164(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_165(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_166(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_167(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_168(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_169(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_170(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_171(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_172(void);
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_173(void);


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[175] = {
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
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
	::ChaosAbiMethodEntryV0 entries[174];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		174u,
		0u,
		3868551925u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_0
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_1
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_2
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_3
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_4
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_5
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_6
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_7
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_8
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_9
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_10
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_11
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_12
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_13
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_14
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_15
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_16
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_17
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_18
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_19
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_20
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_21
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_22
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_23
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_24
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_25
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_26
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_27
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_28
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_29
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_30
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_31
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_32
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_33
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_34
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_35
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_36
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_37
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_38
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_39
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_40
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_41
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_42
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_43
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_44
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_45
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_46
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_47
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_48
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_49
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_50
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_51
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_52
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_53
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_54
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_55
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_56
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_57
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_58
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_59
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_60
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_61
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_62
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_63
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_64
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_65
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_66
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_67
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_68
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_69
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_70
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_71
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_72
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_73
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_74
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_75
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_76
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_77
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_78
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_79
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_80
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_81
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_82
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_83
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_84
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_85
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_86
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_87
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_88
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_89
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_90
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_91
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_92
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_93
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_94
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_95
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_96
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_97
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_98
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_99
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_100
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_101
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_102
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_103
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_104
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_105
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_106
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_107
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_108
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_109
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_110
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_111
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_112
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_113
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_114
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_115
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_116
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_117
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_118
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_119
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_120
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_121
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_122
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_123
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_124
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_125
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_126
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_127
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_128
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_129
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_130
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_131
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_132
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_133
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_134
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_135
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_136
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_137
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_138
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_139
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_140
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_141
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_142
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_143
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_144
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_145
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_146
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_147
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_148
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_149
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_150
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_151
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_152
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_153
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_154
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_155
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_156
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_157
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_158
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_159
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_160
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_161
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_162
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_163
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_164
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_165
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_166
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_167
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_168
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_169
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_170
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_171
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_172
		{ 0u, 0u },  // NodesSubjects_NodesSubjects_CustomEntrySubject_173
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
		"NodesSubjects",
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
		/* .name_utf8         = */ "NodesSubjects",
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
		::chaos::il2cpp::runtime_core::RegisterModule("NodesSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[174] = {
	{ "CustomEntrySubject_0", 0x00000003u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_1", 0x00000004u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_2", 0x00000005u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_3", 0x00000006u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_4", 0x00000007u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_5", 0x00000008u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_6", 0x00000009u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_7", 0x0000000Au, 0u },  // NodesSubjects
	{ "CustomEntrySubject_8", 0x0000000Bu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_9", 0x0000000Cu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_10", 0x0000000Du, 0u },  // NodesSubjects
	{ "CustomEntrySubject_11", 0x0000000Eu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_12", 0x0000000Fu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_13", 0x00000010u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_14", 0x00000011u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_15", 0x00000012u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_16", 0x00000013u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_17", 0x00000014u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_18", 0x00000015u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_19", 0x00000016u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_20", 0x00000017u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_21", 0x00000018u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_22", 0x00000019u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_23", 0x0000001Au, 0u },  // NodesSubjects
	{ "CustomEntrySubject_24", 0x0000001Bu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_25", 0x0000001Cu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_26", 0x0000001Du, 0u },  // NodesSubjects
	{ "CustomEntrySubject_27", 0x0000001Eu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_28", 0x0000001Fu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_29", 0x00000020u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_30", 0x00000021u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_31", 0x00000022u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_32", 0x00000023u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_33", 0x00000024u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_34", 0x00000025u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_35", 0x00000026u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_36", 0x00000027u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_37", 0x00000028u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_38", 0x00000029u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_39", 0x0000002Au, 0u },  // NodesSubjects
	{ "CustomEntrySubject_40", 0x0000002Bu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_41", 0x0000002Cu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_42", 0x0000002Du, 0u },  // NodesSubjects
	{ "CustomEntrySubject_43", 0x0000002Eu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_44", 0x0000002Fu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_45", 0x00000030u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_46", 0x00000031u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_47", 0x00000032u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_48", 0x00000033u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_49", 0x00000034u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_50", 0x00000035u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_51", 0x00000036u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_52", 0x00000037u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_53", 0x00000038u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_54", 0x00000039u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_55", 0x0000003Au, 0u },  // NodesSubjects
	{ "CustomEntrySubject_56", 0x0000003Bu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_57", 0x0000003Cu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_58", 0x0000003Du, 0u },  // NodesSubjects
	{ "CustomEntrySubject_59", 0x0000003Eu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_60", 0x0000003Fu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_61", 0x00000040u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_62", 0x00000041u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_63", 0x00000042u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_64", 0x00000043u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_65", 0x00000044u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_66", 0x00000045u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_67", 0x00000046u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_68", 0x00000047u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_69", 0x00000048u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_70", 0x00000049u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_71", 0x0000004Au, 0u },  // NodesSubjects
	{ "CustomEntrySubject_72", 0x0000004Bu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_73", 0x0000004Cu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_74", 0x0000004Du, 0u },  // NodesSubjects
	{ "CustomEntrySubject_75", 0x0000004Eu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_76", 0x0000004Fu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_77", 0x00000050u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_78", 0x00000051u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_79", 0x00000052u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_80", 0x00000053u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_81", 0x00000054u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_82", 0x00000055u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_83", 0x00000056u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_84", 0x00000057u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_85", 0x00000058u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_86", 0x00000059u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_87", 0x0000005Au, 0u },  // NodesSubjects
	{ "CustomEntrySubject_88", 0x0000005Bu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_89", 0x0000005Cu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_90", 0x0000005Du, 0u },  // NodesSubjects
	{ "CustomEntrySubject_91", 0x0000005Eu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_92", 0x0000005Fu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_93", 0x00000060u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_94", 0x00000061u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_95", 0x00000062u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_96", 0x00000063u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_97", 0x00000064u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_98", 0x00000065u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_99", 0x00000066u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_100", 0x00000067u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_101", 0x00000068u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_102", 0x00000069u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_103", 0x0000006Au, 0u },  // NodesSubjects
	{ "CustomEntrySubject_104", 0x0000006Bu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_105", 0x0000006Cu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_106", 0x0000006Du, 0u },  // NodesSubjects
	{ "CustomEntrySubject_107", 0x0000006Eu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_108", 0x0000006Fu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_109", 0x00000070u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_110", 0x00000071u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_111", 0x00000072u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_112", 0x00000073u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_113", 0x00000074u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_114", 0x00000075u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_115", 0x00000076u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_116", 0x00000077u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_117", 0x00000078u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_118", 0x00000079u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_119", 0x0000007Au, 0u },  // NodesSubjects
	{ "CustomEntrySubject_120", 0x0000007Bu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_121", 0x0000007Cu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_122", 0x0000007Du, 0u },  // NodesSubjects
	{ "CustomEntrySubject_123", 0x0000007Eu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_124", 0x0000007Fu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_125", 0x00000080u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_126", 0x00000081u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_127", 0x00000082u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_128", 0x00000083u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_129", 0x00000084u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_130", 0x00000085u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_131", 0x00000086u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_132", 0x00000087u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_133", 0x00000088u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_134", 0x00000089u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_135", 0x0000008Au, 0u },  // NodesSubjects
	{ "CustomEntrySubject_136", 0x0000008Bu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_137", 0x0000008Cu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_138", 0x0000008Du, 0u },  // NodesSubjects
	{ "CustomEntrySubject_139", 0x0000008Eu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_140", 0x0000008Fu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_141", 0x00000090u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_142", 0x00000091u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_143", 0x00000092u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_144", 0x00000093u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_145", 0x00000094u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_146", 0x00000095u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_147", 0x00000096u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_148", 0x00000097u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_149", 0x00000098u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_150", 0x00000099u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_151", 0x0000009Au, 0u },  // NodesSubjects
	{ "CustomEntrySubject_152", 0x0000009Bu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_153", 0x0000009Cu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_154", 0x0000009Du, 0u },  // NodesSubjects
	{ "CustomEntrySubject_155", 0x0000009Eu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_156", 0x0000009Fu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_157", 0x000000A0u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_158", 0x000000A1u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_159", 0x000000A2u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_160", 0x000000A3u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_161", 0x000000A4u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_162", 0x000000A5u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_163", 0x000000A6u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_164", 0x000000A7u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_165", 0x000000A8u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_166", 0x000000A9u, 0u },  // NodesSubjects
	{ "CustomEntrySubject_167", 0x000000AAu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_168", 0x000000ABu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_169", 0x000000ACu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_170", 0x000000ADu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_171", 0x000000AEu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_172", 0x000000AFu, 0u },  // NodesSubjects
	{ "CustomEntrySubject_173", 0x000000B0u, 0u },  // NodesSubjects
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {
	{ "NodesSubjects", "", 0u, 174u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[174] = {
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
	{ 0x00000081u, 126u },
	{ 0x00000082u, 127u },
	{ 0x00000083u, 128u },
	{ 0x00000084u, 129u },
	{ 0x00000085u, 130u },
	{ 0x00000086u, 131u },
	{ 0x00000087u, 132u },
	{ 0x00000088u, 133u },
	{ 0x00000089u, 134u },
	{ 0x0000008Au, 135u },
	{ 0x0000008Bu, 136u },
	{ 0x0000008Cu, 137u },
	{ 0x0000008Du, 138u },
	{ 0x0000008Eu, 139u },
	{ 0x0000008Fu, 140u },
	{ 0x00000090u, 141u },
	{ 0x00000091u, 142u },
	{ 0x00000092u, 143u },
	{ 0x00000093u, 144u },
	{ 0x00000094u, 145u },
	{ 0x00000095u, 146u },
	{ 0x00000096u, 147u },
	{ 0x00000097u, 148u },
	{ 0x00000098u, 149u },
	{ 0x00000099u, 150u },
	{ 0x0000009Au, 151u },
	{ 0x0000009Bu, 152u },
	{ 0x0000009Cu, 153u },
	{ 0x0000009Du, 154u },
	{ 0x0000009Eu, 155u },
	{ 0x0000009Fu, 156u },
	{ 0x000000A0u, 157u },
	{ 0x000000A1u, 158u },
	{ 0x000000A2u, 159u },
	{ 0x000000A3u, 160u },
	{ 0x000000A4u, 161u },
	{ 0x000000A5u, 162u },
	{ 0x000000A6u, 163u },
	{ 0x000000A7u, 164u },
	{ 0x000000A8u, 165u },
	{ 0x000000A9u, 166u },
	{ 0x000000AAu, 167u },
	{ 0x000000ABu, 168u },
	{ 0x000000ACu, 169u },
	{ 0x000000ADu, 170u },
	{ 0x000000AEu, 171u },
	{ 0x000000AFu, 172u },
	{ 0x000000B0u, 173u },
};

// Dispatch table (function pointers)
static HotpatchEntryV0 s_hotpatch_entries[174] = {
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_0
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_1
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_2
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_3), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_3
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_4), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_4
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_5), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_5
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_6), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_6
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_7), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_7
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_8), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_8
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_9), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_9
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_10), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_10
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_11), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_11
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_12), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_12
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_13), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_13
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_14), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_14
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_15), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_15
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_16), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_16
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_17), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_17
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_18), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_18
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_19), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_19
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_20), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_20
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_21), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_21
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_22), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_22
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_23), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_23
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_24), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_24
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_25), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_25
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_26), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_26
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_27), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_27
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_28), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_28
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_29), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_29
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_30), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_30
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_31), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_31
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_32), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_32
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_33), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_33
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_34), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_34
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_35), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_35
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_36), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_36
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_37), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_37
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_38), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_38
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_39), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_39
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_40), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_40
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_41), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_41
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_42), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_42
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_43), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_43
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_44), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_44
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_45), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_45
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_46), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_46
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_47), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_47
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_48), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_48
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_49), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_49
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_50), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_50
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_51), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_51
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_52), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_52
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_53), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_53
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_54), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_54
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_55), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_55
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_56), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_56
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_57), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_57
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_58), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_58
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_59), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_59
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_60), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_60
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_61), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_61
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_62), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_62
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_63), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_63
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_64), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_64
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_65), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_65
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_66), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_66
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_67), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_67
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_68), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_68
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_69), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_69
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_70), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_70
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_71), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_71
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_72), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_72
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_73), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_73
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_74), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_74
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_75), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_75
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_76), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_76
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_77), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_77
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_78), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_78
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_79), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_79
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_80), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_80
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_81), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_81
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_82), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_82
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_83), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_83
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_84), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_84
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_85), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_85
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_86), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_86
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_87), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_87
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_88), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_88
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_89), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_89
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_90), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_90
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_91), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_91
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_92), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_92
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_93), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_93
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_94), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_94
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_95), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_95
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_96), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_96
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_97), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_97
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_98), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_98
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_99), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_99
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_100), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_100
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_101), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_101
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_102), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_102
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_103), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_103
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_104), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_104
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_105), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_105
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_106), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_106
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_107), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_107
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_108), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_108
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_109), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_109
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_110), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_110
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_111), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_111
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_112), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_112
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_113), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_113
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_114), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_114
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_115), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_115
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_116), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_116
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_117), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_117
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_118), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_118
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_119), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_119
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_120), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_120
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_121), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_121
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_122), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_122
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_123), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_123
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_124), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_124
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_125), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_125
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_126), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_126
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_127), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_127
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_128), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_128
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_129), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_129
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_130), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_130
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_131), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_131
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_132), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_132
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_133), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_133
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_134), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_134
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_135), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_135
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_136), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_136
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_137), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_137
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_138), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_138
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_139), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_139
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_140), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_140
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_141), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_141
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_142), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_142
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_143), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_143
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_144), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_144
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_145), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_145
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_146), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_146
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_147), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_147
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_148), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_148
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_149), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_149
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_150), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_150
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_151), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_151
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_152), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_152
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_153), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_153
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_154), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_154
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_155), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_155
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_156), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_156
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_157), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_157
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_158), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_158
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_159), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_159
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_160), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_160
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_161), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_161
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_162), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_162
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_163), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_163
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_164), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_164
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_165), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_165
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_166), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_166
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_167), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_167
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_168), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_168
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_169), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_169
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_170), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_170
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_171), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_171
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_172), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_172
	{ reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_173), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // NodesSubjects::CustomEntrySubject_173
};

// Module hotpatch bundle
static constexpr HotpatchModuleV0 s_hotpatch_module = {
	"NodesSubjects",
	s_hotpatch_types,
	1u,
	s_hotpatch_methods,
	174u,
	s_hotpatch_slots,
	174u,
	s_hotpatch_entries,
	174u,
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
static void (*kAotMethods[174])() = {
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_0),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_1),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_2),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_3),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_4),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_5),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_6),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_7),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_8),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_9),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_10),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_11),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_12),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_13),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_14),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_15),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_16),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_17),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_18),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_19),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_20),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_21),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_22),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_23),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_24),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_25),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_26),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_27),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_28),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_29),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_30),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_31),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_32),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_33),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_34),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_35),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_36),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_37),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_38),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_39),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_40),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_41),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_42),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_43),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_44),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_45),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_46),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_47),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_48),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_49),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_50),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_51),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_52),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_53),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_54),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_55),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_56),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_57),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_58),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_59),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_60),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_61),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_62),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_63),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_64),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_65),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_66),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_67),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_68),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_69),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_70),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_71),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_72),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_73),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_74),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_75),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_76),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_77),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_78),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_79),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_80),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_81),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_82),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_83),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_84),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_85),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_86),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_87),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_88),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_89),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_90),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_91),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_92),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_93),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_94),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_95),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_96),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_97),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_98),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_99),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_100),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_101),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_102),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_103),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_104),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_105),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_106),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_107),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_108),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_109),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_110),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_111),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_112),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_113),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_114),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_115),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_116),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_117),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_118),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_119),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_120),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_121),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_122),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_123),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_124),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_125),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_126),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_127),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_128),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_129),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_130),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_131),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_132),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_133),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_134),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_135),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_136),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_137),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_138),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_139),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_140),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_141),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_142),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_143),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_144),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_145),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_146),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_147),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_148),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_149),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_150),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_151),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_152),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_153),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_154),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_155),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_156),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_157),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_158),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_159),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_160),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_161),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_162),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_163),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_164),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_165),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_166),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_167),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_168),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_169),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_170),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_171),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_172),
	reinterpret_cast<void(*)()>(&NodesSubjects_NodesSubjects_CustomEntrySubject_173),
};

// ── Benchmark wrappers (kBenchmarkWrappers[]) ──────────────────────────
// Each wrapper supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_benchmark_this_sentinel = 0;
extern "C" void (*kBenchmarkWrappers[174])() = {
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
	[]() {kAotMethods[126]();},
	[]() {kAotMethods[127]();},
	[]() {kAotMethods[128]();},
	[]() {kAotMethods[129]();},
	[]() {kAotMethods[130]();},
	[]() {kAotMethods[131]();},
	[]() {kAotMethods[132]();},
	[]() {kAotMethods[133]();},
	[]() {kAotMethods[134]();},
	[]() {kAotMethods[135]();},
	[]() {kAotMethods[136]();},
	[]() {kAotMethods[137]();},
	[]() {kAotMethods[138]();},
	[]() {kAotMethods[139]();},
	[]() {kAotMethods[140]();},
	[]() {kAotMethods[141]();},
	[]() {kAotMethods[142]();},
	[]() {kAotMethods[143]();},
	[]() {kAotMethods[144]();},
	[]() {kAotMethods[145]();},
	[]() {kAotMethods[146]();},
	[]() {kAotMethods[147]();},
	[]() {kAotMethods[148]();},
	[]() {kAotMethods[149]();},
	[]() {kAotMethods[150]();},
	[]() {kAotMethods[151]();},
	[]() {kAotMethods[152]();},
	[]() {kAotMethods[153]();},
	[]() {kAotMethods[154]();},
	[]() {kAotMethods[155]();},
	[]() {kAotMethods[156]();},
	[]() {kAotMethods[157]();},
	[]() {kAotMethods[158]();},
	[]() {kAotMethods[159]();},
	[]() {kAotMethods[160]();},
	[]() {kAotMethods[161]();},
	[]() {kAotMethods[162]();},
	[]() {kAotMethods[163]();},
	[]() {kAotMethods[164]();},
	[]() {kAotMethods[165]();},
	[]() {kAotMethods[166]();},
	[]() {kAotMethods[167]();},
	[]() {kAotMethods[168]();},
	[]() {kAotMethods[169]();},
	[]() {kAotMethods[170]();},
	[]() {kAotMethods[171]();},
	[]() {kAotMethods[172]();},
	[]() {kAotMethods[173]();},
};

// ── Subject entry index mapping ─────────────────────────────────
// Maps subject index (0-based sequential) to kAotMethod index.
// Used by runtime-entry.cpp to route --benchmark N to the correct
// AOT method slot, since kAotMethods[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 174;
extern "C" const int kSubjectEntryIndices[174] = {
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
	126,
	127,
	128,
	129,
	130,
	131,
	132,
	133,
	134,
	135,
	136,
	137,
	138,
	139,
	140,
	141,
	142,
	143,
	144,
	145,
	146,
	147,
	148,
	149,
	150,
	151,
	152,
	153,
	154,
	155,
	156,
	157,
	158,
	159,
	160,
	161,
	162,
	163,
	164,
	165,
	166,
	167,
	168,
	169,
	170,
	171,
	172,
	173,
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
static void* const kMethodPointers[174] = {
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_0),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_1),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_2),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_3),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_4),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_5),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_6),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_7),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_8),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_9),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_10),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_11),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_12),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_13),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_14),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_15),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_16),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_17),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_18),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_19),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_20),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_21),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_22),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_23),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_24),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_25),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_26),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_27),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_28),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_29),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_30),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_31),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_32),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_33),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_34),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_35),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_36),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_37),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_38),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_39),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_40),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_41),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_42),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_43),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_44),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_45),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_46),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_47),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_48),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_49),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_50),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_51),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_52),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_53),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_54),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_55),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_56),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_57),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_58),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_59),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_60),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_61),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_62),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_63),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_64),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_65),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_66),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_67),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_68),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_69),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_70),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_71),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_72),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_73),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_74),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_75),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_76),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_77),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_78),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_79),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_80),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_81),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_82),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_83),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_84),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_85),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_86),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_87),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_88),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_89),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_90),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_91),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_92),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_93),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_94),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_95),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_96),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_97),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_98),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_99),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_100),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_101),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_102),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_103),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_104),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_105),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_106),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_107),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_108),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_109),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_110),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_111),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_112),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_113),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_114),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_115),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_116),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_117),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_118),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_119),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_120),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_121),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_122),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_123),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_124),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_125),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_126),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_127),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_128),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_129),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_130),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_131),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_132),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_133),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_134),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_135),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_136),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_137),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_138),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_139),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_140),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_141),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_142),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_143),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_144),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_145),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_146),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_147),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_148),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_149),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_150),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_151),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_152),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_153),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_154),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_155),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_156),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_157),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_158),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_159),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_160),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_161),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_162),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_163),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_164),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_165),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_166),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_167),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_168),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_169),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_170),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_171),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_172),
	reinterpret_cast<void*>(&NodesSubjects_NodesSubjects_CustomEntrySubject_173),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 174u,
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
	.image_name_utf8    = "NodesSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_NodesSubjects_NodesSubjects[1] = {
	{ 0u, "NodesSubjects/NodesSubjects::_exitCode", "_exitCode", "System.Int32", 0LL },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_NodesSubjects_NodesSubjects[174] = {
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_0:System.Void()", "CustomEntrySubject_0", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_1:System.Void()", "CustomEntrySubject_1", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_2:System.Void()", "CustomEntrySubject_2", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_3:System.Void()", "CustomEntrySubject_3", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_4:System.Void()", "CustomEntrySubject_4", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_5:System.Void()", "CustomEntrySubject_5", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_6:System.Void()", "CustomEntrySubject_6", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_7:System.Void()", "CustomEntrySubject_7", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_8:System.Void()", "CustomEntrySubject_8", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_9:System.Void()", "CustomEntrySubject_9", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_10:System.Void()", "CustomEntrySubject_10", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_11:System.Void()", "CustomEntrySubject_11", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_12:System.Void()", "CustomEntrySubject_12", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_13:System.Void()", "CustomEntrySubject_13", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_14:System.Void()", "CustomEntrySubject_14", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_15:System.Void()", "CustomEntrySubject_15", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_16:System.Void()", "CustomEntrySubject_16", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_17:System.Void()", "CustomEntrySubject_17", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_18:System.Void()", "CustomEntrySubject_18", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_19:System.Void()", "CustomEntrySubject_19", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_20:System.Void()", "CustomEntrySubject_20", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_21:System.Void()", "CustomEntrySubject_21", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_22:System.Void()", "CustomEntrySubject_22", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_23:System.Void()", "CustomEntrySubject_23", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_24:System.Void()", "CustomEntrySubject_24", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_25:System.Void()", "CustomEntrySubject_25", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_26:System.Void()", "CustomEntrySubject_26", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_27:System.Void()", "CustomEntrySubject_27", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_28:System.Void()", "CustomEntrySubject_28", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_29:System.Void()", "CustomEntrySubject_29", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_30:System.Void()", "CustomEntrySubject_30", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_31:System.Void()", "CustomEntrySubject_31", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_32:System.Void()", "CustomEntrySubject_32", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_33:System.Void()", "CustomEntrySubject_33", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_34:System.Void()", "CustomEntrySubject_34", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_35:System.Void()", "CustomEntrySubject_35", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_36:System.Void()", "CustomEntrySubject_36", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_37:System.Void()", "CustomEntrySubject_37", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_38:System.Void()", "CustomEntrySubject_38", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_39:System.Void()", "CustomEntrySubject_39", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_40:System.Void()", "CustomEntrySubject_40", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_41:System.Void()", "CustomEntrySubject_41", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_42:System.Void()", "CustomEntrySubject_42", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_43:System.Void()", "CustomEntrySubject_43", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_44:System.Void()", "CustomEntrySubject_44", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_45:System.Void()", "CustomEntrySubject_45", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_46:System.Void()", "CustomEntrySubject_46", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_47:System.Void()", "CustomEntrySubject_47", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_48:System.Void()", "CustomEntrySubject_48", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_49:System.Void()", "CustomEntrySubject_49", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_50:System.Void()", "CustomEntrySubject_50", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_51:System.Void()", "CustomEntrySubject_51", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_52:System.Void()", "CustomEntrySubject_52", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_53:System.Void()", "CustomEntrySubject_53", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_54:System.Void()", "CustomEntrySubject_54", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_55:System.Void()", "CustomEntrySubject_55", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_56:System.Void()", "CustomEntrySubject_56", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_57:System.Void()", "CustomEntrySubject_57", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_58:System.Void()", "CustomEntrySubject_58", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_59:System.Void()", "CustomEntrySubject_59", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_60:System.Void()", "CustomEntrySubject_60", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_61:System.Void()", "CustomEntrySubject_61", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_62:System.Void()", "CustomEntrySubject_62", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_63:System.Void()", "CustomEntrySubject_63", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_64:System.Void()", "CustomEntrySubject_64", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_65:System.Void()", "CustomEntrySubject_65", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_66:System.Void()", "CustomEntrySubject_66", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_67:System.Void()", "CustomEntrySubject_67", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_68:System.Void()", "CustomEntrySubject_68", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_69:System.Void()", "CustomEntrySubject_69", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_70:System.Void()", "CustomEntrySubject_70", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_71:System.Void()", "CustomEntrySubject_71", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_72:System.Void()", "CustomEntrySubject_72", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_73:System.Void()", "CustomEntrySubject_73", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_74:System.Void()", "CustomEntrySubject_74", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_75:System.Void()", "CustomEntrySubject_75", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_76:System.Void()", "CustomEntrySubject_76", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_77:System.Void()", "CustomEntrySubject_77", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_78:System.Void()", "CustomEntrySubject_78", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_79:System.Void()", "CustomEntrySubject_79", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_80:System.Void()", "CustomEntrySubject_80", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_81:System.Void()", "CustomEntrySubject_81", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_82:System.Void()", "CustomEntrySubject_82", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_83:System.Void()", "CustomEntrySubject_83", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_84:System.Void()", "CustomEntrySubject_84", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_85:System.Void()", "CustomEntrySubject_85", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_86:System.Void()", "CustomEntrySubject_86", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_87:System.Void()", "CustomEntrySubject_87", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_88:System.Void()", "CustomEntrySubject_88", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_89:System.Void()", "CustomEntrySubject_89", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_90:System.Void()", "CustomEntrySubject_90", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_91:System.Void()", "CustomEntrySubject_91", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_92:System.Void()", "CustomEntrySubject_92", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_93:System.Void()", "CustomEntrySubject_93", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_94:System.Void()", "CustomEntrySubject_94", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_95:System.Void()", "CustomEntrySubject_95", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_96:System.Void()", "CustomEntrySubject_96", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_97:System.Void()", "CustomEntrySubject_97", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_98:System.Void()", "CustomEntrySubject_98", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_99:System.Void()", "CustomEntrySubject_99", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_100:System.Void()", "CustomEntrySubject_100", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_101:System.Void()", "CustomEntrySubject_101", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_102:System.Void()", "CustomEntrySubject_102", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_103:System.Void()", "CustomEntrySubject_103", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_104:System.Void()", "CustomEntrySubject_104", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_105:System.Void()", "CustomEntrySubject_105", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_106:System.Void()", "CustomEntrySubject_106", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_107:System.Void()", "CustomEntrySubject_107", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_108:System.Void()", "CustomEntrySubject_108", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_109:System.Void()", "CustomEntrySubject_109", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_110:System.Void()", "CustomEntrySubject_110", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_111:System.Void()", "CustomEntrySubject_111", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_112:System.Void()", "CustomEntrySubject_112", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_113:System.Void()", "CustomEntrySubject_113", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_114:System.Void()", "CustomEntrySubject_114", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_115:System.Void()", "CustomEntrySubject_115", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_116:System.Void()", "CustomEntrySubject_116", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_117:System.Void()", "CustomEntrySubject_117", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_118:System.Void()", "CustomEntrySubject_118", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_119:System.Void()", "CustomEntrySubject_119", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_120:System.Void()", "CustomEntrySubject_120", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_121:System.Void()", "CustomEntrySubject_121", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_122:System.Void()", "CustomEntrySubject_122", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_123:System.Void()", "CustomEntrySubject_123", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_124:System.Void()", "CustomEntrySubject_124", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_125:System.Void()", "CustomEntrySubject_125", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_126:System.Void()", "CustomEntrySubject_126", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_127:System.Void()", "CustomEntrySubject_127", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_128:System.Void()", "CustomEntrySubject_128", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_129:System.Void()", "CustomEntrySubject_129", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_130:System.Void()", "CustomEntrySubject_130", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_131:System.Void()", "CustomEntrySubject_131", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_132:System.Void()", "CustomEntrySubject_132", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_133:System.Void()", "CustomEntrySubject_133", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_134:System.Void()", "CustomEntrySubject_134", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_135:System.Void()", "CustomEntrySubject_135", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_136:System.Void()", "CustomEntrySubject_136", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_137:System.Void()", "CustomEntrySubject_137", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_138:System.Void()", "CustomEntrySubject_138", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_139:System.Void()", "CustomEntrySubject_139", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_140:System.Void()", "CustomEntrySubject_140", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_141:System.Void()", "CustomEntrySubject_141", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_142:System.Void()", "CustomEntrySubject_142", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_143:System.Void()", "CustomEntrySubject_143", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_144:System.Void()", "CustomEntrySubject_144", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_145:System.Void()", "CustomEntrySubject_145", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_146:System.Void()", "CustomEntrySubject_146", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_147:System.Void()", "CustomEntrySubject_147", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_148:System.Void()", "CustomEntrySubject_148", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_149:System.Void()", "CustomEntrySubject_149", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_150:System.Void()", "CustomEntrySubject_150", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_151:System.Void()", "CustomEntrySubject_151", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_152:System.Void()", "CustomEntrySubject_152", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_153:System.Void()", "CustomEntrySubject_153", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_154:System.Void()", "CustomEntrySubject_154", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_155:System.Void()", "CustomEntrySubject_155", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_156:System.Void()", "CustomEntrySubject_156", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_157:System.Void()", "CustomEntrySubject_157", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_158:System.Void()", "CustomEntrySubject_158", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_159:System.Void()", "CustomEntrySubject_159", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_160:System.Void()", "CustomEntrySubject_160", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_161:System.Void()", "CustomEntrySubject_161", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_162:System.Void()", "CustomEntrySubject_162", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_163:System.Void()", "CustomEntrySubject_163", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_164:System.Void()", "CustomEntrySubject_164", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_165:System.Void()", "CustomEntrySubject_165", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_166:System.Void()", "CustomEntrySubject_166", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_167:System.Void()", "CustomEntrySubject_167", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_168:System.Void()", "CustomEntrySubject_168", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_169:System.Void()", "CustomEntrySubject_169", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_170:System.Void()", "CustomEntrySubject_170", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_171:System.Void()", "CustomEntrySubject_171", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_172:System.Void()", "CustomEntrySubject_172", "System.Void", 0, nullptr, 0u },
	{ 0u, "NodesSubjects/NodesSubjects::CustomEntrySubject_173:System.Void()", "CustomEntrySubject_173", "System.Void", 0, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[1] = {
	{ 0u, "NodesSubjects/NodesSubjects", "NodesSubjects/NodesSubjects", "", "NodesSubjects", "NodesSubjects", nullptr, kReflFields_NodesSubjects_NodesSubjects, 1u, nullptr, 0u,
	kReflMethods_NodesSubjects_NodesSubjects, 174u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {
	&kReflTypes[0],
};

static const ReflectionQueryImageDescriptor kReflImage = { "NodesSubjects", kReflTypePtrs, 1u };

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
// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_0()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_1()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_2()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_3()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_4()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_5()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_6()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_7()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_8()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_9()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_10()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_11()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_12()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_13()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_14()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_14(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_15()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_15(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_16()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_16(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_17()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_17(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_18()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_18(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_19()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_19(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_20()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_20(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_21()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_21(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_22()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_22(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_23()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_23(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_24()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_24(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_25()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_25(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_26()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_26(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_27()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_27(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_28()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_28(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_29()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_29(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_30()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_30(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_31()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_31(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_32()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_32(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_33()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_33(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_34()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_34(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_35()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_35(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_36()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_36(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_37()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_37(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_38()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_38(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_39()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_39(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_40()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_40(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_41()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_41(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_42()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_42(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_43()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_43(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_44()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_44(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_45()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_45(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_46()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_46(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_47()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_47(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_48()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_48(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_49()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_49(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_50()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_50(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_51()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_51(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_52()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_52(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_53()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_53(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_54()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_54(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_55()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_55(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_56()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_56(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_57()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_57(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_58()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_58(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_59()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_59(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_60()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_60(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_61()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_61(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_62()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_62(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_63()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_63(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_64()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_64(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_65()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_65(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_66()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_66(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_67()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_67(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_68()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_68(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_69()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_69(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_70()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_70(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_71()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_71(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_72()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_72(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_73()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_73(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_74()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_74(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_75()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_75(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_76()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_76(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_77()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_77(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_78()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_78(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_79()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_79(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_80()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_80(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_81()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_81(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_82()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_82(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_83()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_83(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_84()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_84(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_85()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_85(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_86()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_86(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_87()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_87(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_88()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_88(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_89()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_89(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_90()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_90(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_91()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_91(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_92()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_92(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_93()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_93(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_94()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_94(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_95()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_95(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_96()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_96(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_97()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_97(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_98()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_98(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_99()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_99(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_100()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_100(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_101()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_101(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_102()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_102(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_103()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_103(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_104()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_104(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_105()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_105(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_106()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_106(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_107()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_107(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_108()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_108(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_109()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_109(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_110()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_110(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_111()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_111(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_112()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_112(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_113()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_113(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_114()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_114(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_115()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_115(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_116()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_116(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_117()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_117(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_118()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_118(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_119()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_119(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_120()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_120(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_121()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_121(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_122()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_122(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_123()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_123(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_124()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_124(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_125()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_125(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_126()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_126(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_127()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_127(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_128()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_128(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_129()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_129(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_130()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_130(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_131()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_131(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_132()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_132(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_133()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_133(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_134()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_134(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_135()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_135(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_136()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_136(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_137()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_137(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_138()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_138(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_139()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_139(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_140()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_140(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_141()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_141(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_142()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_142(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_143()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_143(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_144()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_144(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_145()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_145(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_146()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_146(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_147()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_147(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_148()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_148(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_149()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_149(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_150()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_150(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_151()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_151(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_152()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_152(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_153()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_153(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_154()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_154(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_155()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_155(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_156()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_156(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_157()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_157(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_158()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_158(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_159()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_159(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_160()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_160(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_161()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_161(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_162()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_162(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_163()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_163(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_164()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_164(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_165()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_165(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_166()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_166(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_167()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_167(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_168()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_168(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_169()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_169(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_170()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_170(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_171()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_171(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_172()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_172(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}

// Managed method: NodesSubjects/NodesSubjects::CustomEntrySubject_173()
extern "C" void NodesSubjects_NodesSubjects_CustomEntrySubject_173(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};


	return;
}



}  // namespace chaos::il2cpp::codegen::NodesSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 174;

extern "C" void ChaosJitRegisterAll() {}