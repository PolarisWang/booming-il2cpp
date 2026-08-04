#include "chaos_pch.h"


#include "async_stubs.h"


#include "exception_stubs.h"


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





extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdelemAllHelper_TestAllElems(void);





// Runtime prelude dependencies included at translation unit level











MethodTable chaos_mt_System_Private_CoreLib_System_Double = {nullptr, nullptr, 1099026126681926114ULL, 0u, 32, 2, 0, nullptr, nullptr, 0, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Double = static_cast<CHAOS_IL2CPP_UINT64>(1099026126681926114ULL);



MethodTable chaos_mt_System_Private_CoreLib_System_Int16 = {nullptr, nullptr, 11007710000311077999ULL, 0u, 32, 2, 0, nullptr, nullptr, 0, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Int16 = static_cast<CHAOS_IL2CPP_UINT64>(11007710000311077999ULL);



MethodTable chaos_mt_System_Private_CoreLib_System_Int64 = {nullptr, nullptr, 11012495074916106146ULL, 0u, 32, 2, 0, nullptr, nullptr, 0, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Int64 = static_cast<CHAOS_IL2CPP_UINT64>(11012495074916106146ULL);



MethodTable chaos_mt_System_Private_CoreLib_System_Int8 = {nullptr, nullptr, 5139897950736228424ULL, 0u, 32, 2, 0, nullptr, nullptr, 0, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Int8 = static_cast<CHAOS_IL2CPP_UINT64>(5139897950736228424ULL);



MethodTable chaos_mt_System_Private_CoreLib_System_Single = {nullptr, nullptr, 4813876735449938439ULL, 0u, 32, 2, 0, nullptr, nullptr, 0, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Single = static_cast<CHAOS_IL2CPP_UINT64>(4813876735449938439ULL);



// ── Virtual method table arrays ──



MethodTable chaos_mt_System_Private_CoreLib_System_Object = {nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_UINT64>(15228727185366376748ULL);



MethodTable chaos_mt_System_Private_CoreLib_System_Delegate = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 7451128447593600616ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_UINT64>(7451128447593600616ULL);



MethodTable chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Delegate), nullptr, 6681393039041505440ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_UINT64>(6681393039041505440ULL);



MethodTable chaos_mt_System_Private_CoreLib_System_Action = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_MulticastDelegate), nullptr, 14722794505377461205ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Action = static_cast<CHAOS_IL2CPP_UINT64>(14722794505377461205ULL);



MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_UINT64>(5474029880995115448ULL);



MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_UINT64>(17082367815459723707ULL);



MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 4197398748135124456ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_MethodBase = static_cast<CHAOS_IL2CPP_UINT64>(4197398748135124456ULL);



MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase), nullptr, 4137207361503509124ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_UINT64>(4137207361503509124ULL);



MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_UINT64>(17040031516751226236ULL);



MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase), nullptr, 10748947813473285525ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_UINT64>(10748947813473285525ULL);



MethodTable chaos_mt_System_Private_CoreLib_System_String = {nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_UINT64>(1782325859292956794ULL);



MethodTable chaos_mt_System_Private_CoreLib_System_Type = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 3222174513575444759ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};



inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Type = static_cast<CHAOS_IL2CPP_UINT64>(3222174513575444759ULL);







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



	if (chaos_runtime_get_abi_v0()->is_string_id(chaos_value))



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











// ── GcTypeLayout pointer offset arrays (computed via offsetof) ──



static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_Delegate[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Delegate, chaos_delegate_target)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Delegate, chaos_delegate_invocation_list))};



static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_MulticastDelegate[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_MulticastDelegate, chaos_delegate_target)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_MulticastDelegate, chaos_delegate_invocation_list))};



static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};



static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};



static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};



static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};



static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};



static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_Type[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_type_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_name_value))};







extern "C" void ChaosRegisterGcLayouts() {



	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();







	// Register managed_array (variable-size: header + contiguous element data).



	registry.Register(CHAOS_IL2CPP_UINT64(chaos_type_id_managed_array), sizeof(chaos_managed_array), nullptr, 0,



		static_cast<CHAOS_IL2CPP_UINT16>(sizeof(CHAOS_IL2CPP_INTPTR)),



		static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_managed_array, length)));







	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);



	registry.Register(7451128447593600616ULL, sizeof(chaos_type_System_Private_CoreLib_System_Delegate), kGcOffsets_chaos_type_System_Private_CoreLib_System_Delegate, 2);



	registry.Register(6681393039041505440ULL, sizeof(chaos_type_System_Private_CoreLib_System_MulticastDelegate), kGcOffsets_chaos_type_System_Private_CoreLib_System_MulticastDelegate, 2);



	registry.Register(14722794505377461205ULL, sizeof(chaos_type_System_Private_CoreLib_System_Action), nullptr, 0);



	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);



	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);



	registry.Register(4197398748135124456ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodBase), nullptr, 0);



	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);



	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);



	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);



	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);



	registry.Register(3222174513575444759ULL, sizeof(chaos_type_System_Private_CoreLib_System_Type), kGcOffsets_chaos_type_System_Private_CoreLib_System_Type, 2);







	// Register MethodTable address ranges for IsValidTypeInfoPointer.



	registry.RegisterTypeInfoRange(reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));



	registry.RegisterTypeInfoRange(reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Delegate), reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Delegate) + sizeof(chaos_mt_System_Private_CoreLib_System_Delegate));



	registry.RegisterTypeInfoRange(reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_MulticastDelegate), reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_MulticastDelegate) + sizeof(chaos_mt_System_Private_CoreLib_System_MulticastDelegate));



	registry.RegisterTypeInfoRange(reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Action), reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Action) + sizeof(chaos_mt_System_Private_CoreLib_System_Action));



	registry.RegisterTypeInfoRange(reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));



	registry.RegisterTypeInfoRange(reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));



	registry.RegisterTypeInfoRange(reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase), reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase));



	registry.RegisterTypeInfoRange(reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));



	registry.RegisterTypeInfoRange(reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));



	registry.RegisterTypeInfoRange(reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));



	registry.RegisterTypeInfoRange(reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));



	registry.RegisterTypeInfoRange(reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Type), reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&chaos_mt_System_Private_CoreLib_System_Type) + sizeof(chaos_mt_System_Private_CoreLib_System_Type));



}







extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Double() noexcept



{



	return ChaosExternalRuntimeFallback("System.Private.CoreLib/System.Double");



}







extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Int16() noexcept



{



	return ChaosExternalRuntimeFallback("System.Private.CoreLib/System.Int16");



}







extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Int64() noexcept



{



	return ChaosExternalRuntimeFallback("System.Private.CoreLib/System.Int64");



}







extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Int8() noexcept



{



	return ChaosExternalRuntimeFallback("System.Private.CoreLib/System.Int8");



}







extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Single() noexcept



{



	return ChaosExternalRuntimeFallback("System.Private.CoreLib/System.Single");



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


static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[2] = {


	0u,


	0u,


};





static constexpr struct {


	::ChaosAbiManifestV0 header;


	::ChaosAbiMethodEntryV0 entries[1];


	CHAOS_IL2CPP_UINT8 params[1];


} s_abi_manifest_storage = {


	{


		CHAOS_ABI_MANIFEST_VERSION,


		1u,


		0u,


		3950255460u,  // FNV-1a over entries+params


		s_abi_manifest_prefix_sum  // O(1) prefix-sum


	},


	{


		{ 1u, 0u },  // SnapshotTestFixtures_LdelemAllHelper_TestAllElems


	},


	{


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











	static const ModuleDescriptor s_native_aot_module = {


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


		::chaos::il2cpp::runtime_core::GetRuntimeAbiV0()->register_module("SnapshotTestFixtures", reinterpret_cast<const struct ModuleDescriptor*>(&s_native_aot_module));


// ── Hotpatch name index + dispatch table ────────────────────


// Method name index entries


static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[1] = {


	{ "TestAllElems", 0x00000002u, 0u },  // LdelemAllHelper


};





// Type name index entries (namespace, short_name)


static constexpr HotpatchTypeEntryV0 s_hotpatch_types[1] = {


	{ "LdelemAllHelper", "", 0u, 1u },


};





// Token→Slot mapping (sorted by token for binary search)


static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[1] = {


	{ 0x00000002u, 0u },


};





// Dispatch table (function pointers)


// s_hotpatch_entries — external linkage for verification_dispatch


// NOTE: `static` is deliberately omitted so that the verification dispatch


// TU (verification_dispatch.generated.cpp) can access this array via


// GetHotpatchEntries(). Without external linkage, the linker cannot resolve


// the symbol across translation units.


extern "C" HotpatchEntryV0 s_hotpatch_entries[1] = {


	{ reinterpret_cast<void*>(&SnapshotTestFixtures_LdelemAllHelper_TestAllElems), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, HotpatchEncodeArgCount(0) },  // LdelemAllHelper::TestAllElems


};





// Accessor functions for verification_dispatch.generated.cpp


extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept { return s_hotpatch_entries; }


extern "C" CHAOS_IL2CPP_INT32 GetHotpatchEntryCount() noexcept { return 1; }





// Module hotpatch bundle (extern linkage — referenced across page files)


extern constexpr HotpatchModuleV0 s_hotpatch_module = {


	"SnapshotTestFixtures",


	s_hotpatch_types,


	1u,


	s_hotpatch_methods,


	1u,


	s_hotpatch_slots,


	1u,


	s_hotpatch_entries,


	1u,


};





// Expose hotpatch module to BootstrapRuntime


extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module


	= &s_hotpatch_module;


// --- External Runtime Dispatch Table (empty) ---


extern "C" const char* kChaosExternalRuntimeSubjects[1] = { nullptr };


extern "C" void* kChaosExternalRuntimeFnTable[1] = { nullptr };


extern "C" CHAOS_IL2CPP_INT32 kChaosExternalRuntimeCount = 0;





// (no method AOT entries for this module)


// ── Method table (kMethodTable[]) ──────────────────────────────


// const function pointer array for dispatch via slot index.


// NOTE: Pure data only — dispatch routing is in <chaos/hotpatch_dispatch.h>.


// NOTE: Emitted in ALL modes (AOT + JIT). JIT mode verification dispatch


// uses kDefaultArgThunks to bypass JIT precode trampoline (verification


// tests AOT codegen, not JIT compilation).


static void (*kMethodTable[1])() = {


	reinterpret_cast<void(*)()>(&SnapshotTestFixtures_LdelemAllHelper_TestAllElems),


};





// ── Default-arg thunks (kDefaultArgThunks[]) ────────────────────────────


// Each thunk supplies default argument values based on parameter types.


// String params receive a valid StringId; all others receive 0.


// Instance methods receive a sentinel this-pointer so they don't crash on null.


static CHAOS_IL2CPP_UINT8 __g_null_instance = 0;


extern "C" void (*kDefaultArgThunks[1])() = {


	[]() {kMethodTable[0]();},


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





	/* ── Entry 0: SnapshotTestFixtures_LdelemAllHelper_TestAllElems ── */


	struct {


		CHAOS_IL2CPP_UINT32 entry_total_size;


		const void*         code_address;


		CHAOS_IL2CPP_UINT32 frame_size;


		CHAOS_IL2CPP_UINT32 num_gc_slots;


		CHAOS_IL2CPP_UINT32 slots[29];


	} entry0;


} kChaosGcSlotMapsSection = {


	/* entry0 = SnapshotTestFixtures_LdelemAllHelper_TestAllElems */


	.entry0 = {


		/* entry_total_size = 136 */ 136u,


		/* code_address */ reinterpret_cast<const void*>(&SnapshotTestFixtures_LdelemAllHelper_TestAllElems),


		/* frame_size = 232 */ 232u,


		/* num_gc_slots = 29 */ 29u,


		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u, 88u, 96u, 104u, 112u, 120u, 128u, 136u, 144u, 152u, 160u, 168u, 176u, 184u, 192u, 200u, 208u, 216u, 224u }


	}


};


#if defined(_MSC_VER)


#pragma pack(pop)


#endif





static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 136u;





// ── CodeRegistrationV0 ─────────────────────────────────────────


// method_pointers: flat array of all AOT function pointers.


static void* const kMethodPointers[1] = {


	reinterpret_cast<void*>(&SnapshotTestFixtures_LdelemAllHelper_TestAllElems),


};





// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)


extern "C" const CodeRegistrationV0 chaos_codegen_code_registration


	= {


	.struct_size               = sizeof(CodeRegistrationV0),


	.method_pointers           = kMethodPointers,


	.method_pointer_count      = 1u,


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


	.image_name_utf8    = "SnapshotTestFixtures",


};


// ── Reflection Query Image Descriptor ──────────────────────────


// Used by ResolveSubjectId to resolve call_target via subjectId


// matching during IR lowering of patched methods.





static constexpr ReflectionQueryMethodDescriptor kReflMethods_SnapshotTestFixtures_LdelemAllHelper[1] = {


	{ 0x00000002u, "SnapshotTestFixtures/LdelemAllHelper::TestAllElems:System.Int32()", "TestAllElems", "System.Int32", 0, nullptr, 0u, nullptr, 0u },


};





static const ReflectionQueryTypeDescriptor kReflTypes[1] = {


	{ 0x00000001u, "SnapshotTestFixtures/LdelemAllHelper", "SnapshotTestFixtures/LdelemAllHelper", "", "LdelemAllHelper", "LdelemAllHelper", nullptr, nullptr, 0u, nullptr, 0u,


nullptr, 0u,    kReflMethods_SnapshotTestFixtures_LdelemAllHelper, 1u, nullptr, 0u, 0u },


};





static const ReflectionQueryTypeDescriptor* kReflTypePtrs[1] = {


	&kReflTypes[0],


};





extern const ReflectionQueryImageDescriptor kReflImage = { "SnapshotTestFixtures", kReflTypePtrs, 1u, 1, 0, 0, 0 };





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


// Managed method: SnapshotTestFixtures/LdelemAllHelper::TestAllElems()


extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdelemAllHelper_TestAllElems(void)


{


	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};


	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 9) chaos_locals{};





	CHAOS_IL2CPP_INTPTR _s0{};


	CHAOS_IL2CPP_INTPTR _s1{};


	CHAOS_IL2CPP_INT64 _i0{};


	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 32) chaos_eval_stack{};


	CHAOS_IL2CPP_SIZE chaos_stack_top = 0;


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);


	_s0 = ChaosArrayNew1D(&chaos_type_info_managed_array.hot, chaos_mt_System_Private_CoreLib_System_Int8.AsTypeInfoHot(), 2, _s0);


	chaos_locals[0] = _s0;


	_acc_0 = _s0;


	_i0 = _acc_0;


	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);


	{


		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);


		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_i0);


		if (chaos_array == nullptr)


		{


			CHAOS_IL2CPP_FAIL_FAST();


		}


		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)


		{


			CHAOS_IL2CPP_FAIL_FAST();


		}


		const auto chaos_element = *reinterpret_cast<CHAOS_IL2CPP_INT8*>(chaos_array_get_elements(chaos_array) + static_cast<CHAOS_IL2CPP_SIZE>(chaos_index));


		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element);


	}


	chaos_locals[1] = _s0;


	_acc_1 = _s0;


	_i0 = _acc_0;


	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);


	{


		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);


		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_i0);


		if (chaos_array == nullptr)


		{


			CHAOS_IL2CPP_FAIL_FAST();


		}


		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)


		{


			CHAOS_IL2CPP_FAIL_FAST();


		}


		const auto chaos_element = *reinterpret_cast<CHAOS_IL2CPP_UINT8*>(chaos_array_get_elements(chaos_array) + static_cast<CHAOS_IL2CPP_SIZE>(chaos_index));


		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element);


	}


	chaos_locals[2] = _s0;


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);


	_s0 = ChaosArrayNew1D(&chaos_type_info_managed_array.hot, chaos_mt_System_Private_CoreLib_System_Int16.AsTypeInfoHot(), 2, _s0);


	chaos_locals[3] = _s0;


	_s0 = chaos_locals[3];


	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);


	{


		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);


		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);


		if (chaos_array == nullptr)


		{


			CHAOS_IL2CPP_FAIL_FAST();


		}


		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)


		{


			CHAOS_IL2CPP_FAIL_FAST();


		}


		const auto chaos_element = *reinterpret_cast<CHAOS_IL2CPP_INT16*>(chaos_array_get_elements(chaos_array) + static_cast<CHAOS_IL2CPP_SIZE>(chaos_index));


		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element);


	}


	chaos_locals[4] = _s0;


	_s0 = chaos_locals[3];


	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);


	{


		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);


		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);


		if (chaos_array == nullptr)


		{


			CHAOS_IL2CPP_FAIL_FAST();


		}


		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)


		{


			CHAOS_IL2CPP_FAIL_FAST();


		}


		const auto chaos_element = *reinterpret_cast<CHAOS_IL2CPP_UINT16*>(chaos_array_get_elements(chaos_array) + static_cast<CHAOS_IL2CPP_SIZE>(chaos_index));


		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element);


	}


	chaos_locals[5] = _s0;


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);


	_s0 = ChaosArrayNew1D(&chaos_type_info_managed_array.hot, chaos_mt_System_Private_CoreLib_System_Int64.AsTypeInfoHot(), 2, _s0);


	chaos_locals[6] = _s0;


	_s0 = chaos_locals[6];


	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);


	{


		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);


		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);


		if (chaos_array == nullptr)


		{


			CHAOS_IL2CPP_FAIL_FAST();


		}


		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)


		{


			CHAOS_IL2CPP_FAIL_FAST();


		}


		const auto chaos_element = chaos_array_get_elements(chaos_array)[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];


		_s0 = static_cast<CHAOS_IL2CPP_INT64>(chaos_element);


	}


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);


	_s0 = ChaosArrayNew1D(&chaos_type_info_managed_array.hot, chaos_mt_System_Private_CoreLib_System_Single.AsTypeInfoHot(), 2, _s0);


	chaos_locals[7] = ChaosStoreInt64(_s0);


	_s0 = chaos_locals[7];


	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);


	{


		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);


		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);


		if (chaos_array == nullptr)


		{


			CHAOS_IL2CPP_FAIL_FAST();


		}


		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)


		{


			CHAOS_IL2CPP_FAIL_FAST();


		}


		const auto chaos_element = chaos_array_get_elements(chaos_array)[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];


		_s0 = ChaosStoreFloat32(ChaosLoadFloat32(chaos_element));


	}


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(3);


	_s0 = ChaosArrayNew1D(&chaos_type_info_managed_array.hot, chaos_mt_System_Private_CoreLib_System_Double.AsTypeInfoHot(), 2, _s0);


	chaos_locals[8] = ChaosStoreFloat32(_s0);


	_s0 = chaos_locals[8];


	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);


	{


		const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s1);


		auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s0);


		if (chaos_array == nullptr)


		{


			CHAOS_IL2CPP_FAIL_FAST();


		}


		if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)


		{


			CHAOS_IL2CPP_FAIL_FAST();


		}


		const auto chaos_element = chaos_array_get_elements(chaos_array)[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];


		_s0 = ChaosStoreFloat64(ChaosLoadFloat64(chaos_element));


	}


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);


	return static_cast<CHAOS_IL2CPP_INT32>(_s0);


	return {};


}











}  // namespace chaos::il2cpp::codegen::SnapshotTestFixtures


#pragma warning(pop)





// extern "C" definition for link-time visibility from runtime-entry.cpp

extern "C" const int kAotMethodCount = 1;




extern "C" void ChaosJitRegisterAll() {}





// Forward declaration for entry symbol (defined in codegen namespace above).


extern "C" CHAOS_IL2CPP_INT32 SnapshotTestFixtures_LdelemAllHelper_TestAllElems(CHAOS_IL2CPP_INT32);


extern "C" CHAOS_IL2CPP_INT32 RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex) {


    return SnapshotTestFixtures_LdelemAllHelper_TestAllElems(entryIndex);


}