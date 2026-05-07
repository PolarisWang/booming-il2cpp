#include <chaos/common.h>
#include "runtime_core.h"
#include "codegen_bridge.h"
#include "module_registry.h"
#include "abi_manifest.h"
#include "dispatch_table.h"
#include "runtime_vtable.h"
#include "runtime_instantiation.h"
extern "C" void InterpreterEntryDirect(CHAOS_IL2CPP_UINTPTR, void*, void*) noexcept;
#pragma warning(push)
#pragma warning(disable: 4065 4244)
#include <chaos/common.h>
#include <chaos/type_info.h>
	return ChaosDatetimeGetUtcNow();
}

extern "C" inline void chaos_external_runtime_System_Private_CoreLib_System_Decimal___ctor_System_Void_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1)
{
	chaos_decimal_ctor_int32(chaos_fn_arg_0, chaos_fn_arg_1);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };
static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };
static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };
static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };

extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_fn_arg_0);

// ©¤©¤ ABI manifest ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.
// NOTE: reinterpret_cast is needed because MSVC rejects &anon_struct.header
// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[20] = {
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	0u,
	1u
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[19];
	CHAOS_IL2CPP_UINT8 params[1];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		19u,
		1u,
		172844090u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum,  // O(1) prefix-sum
	},
	{
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method0
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method1
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method2
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method3
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method4
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method5
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method6
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method7
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method8
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method9
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method10
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method11
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method12
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method13
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method14
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method15
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method16
		{ 1u, 0u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Method17
		{ 1u, 1u },  // ConvertCharNativeEntry_ConvertCharNativeEntry_Run
	},
	{
		1u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest = reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);

// ©¤©¤ Module registration ©¤©¤

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[2] = {
		0u,
		1548u,
	};

	static constexpr const char* s_type_names[2] = {
		"<Module>",
		"ConvertCharNativeEntry",
	};

	static constexpr const char* s_type_namespaces[2] = {
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[2] = {
		0u,
		0u,
	};

	static const TypeInfo* const s_type_info_ptrs[2] = {
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
		/* .name_utf8         = */ "ConvertCharNativeEntry",
		/* .image             = */ nullptr,  // Tier 2 metadata ¡ª deferred
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
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 ¡ª deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,

		/* .abi_manifest      = */ s_abi_manifest,
	};

#pragma warning(pop)
extern "C" int __chaos_assert_failures = 0;
extern "C" int RunNativeAot(int i) { return 0; }
