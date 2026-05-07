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
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(65);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Char_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_normalize_native_int_argument(chaos_raw_arg_2);
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_System_Void_System_Int32_System_Int32_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharNativeEntry/ConvertCharNativeEntry::Method3()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	{
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_DateTime__get_UtcNow_System_DateTime__();
		_s0 = ChaosStoreInt64(chaos_result);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_DateTime_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_normalize_native_int_argument(chaos_raw_arg_2);
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_System_Void_System_Int32_System_Int32_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharNativeEntry/ConvertCharNativeEntry::Method4()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_raw_arg_1 = _s0;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		auto* chaos_object = new chaos_type_System_Private_CoreLib_System_Decimal{};
		chaos_object->header.type_info = &chaos_type_info_System_Private_CoreLib_System_Decimal;
		chaos_external_runtime_System_Private_CoreLib_System_Decimal___ctor_System_Void_System_Int32_(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Decimal_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_normalize_native_int_argument(chaos_raw_arg_2);
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_System_Void_System_Int32_System_Int32_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharNativeEntry/ConvertCharNativeEntry::Method5()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = ChaosStoreFloat64(42);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Double_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_normalize_native_int_argument(chaos_raw_arg_2);
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_System_Void_System_Int32_System_Int32_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharNativeEntry/ConvertCharNativeEntry::Method6()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Int16_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_normalize_native_int_argument(chaos_raw_arg_2);
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_System_Void_System_Int32_System_Int32_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharNativeEntry/ConvertCharNativeEntry::Method7()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Int32_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_normalize_native_int_argument(chaos_raw_arg_2);
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_System_Void_System_Int32_System_Int32_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharNativeEntry/ConvertCharNativeEntry::Method8()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Int64_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_normalize_native_int_argument(chaos_raw_arg_2);
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_System_Void_System_Int32_System_Int32_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharNativeEntry/ConvertCharNativeEntry::Method9()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Int32_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_normalize_native_int_argument(chaos_raw_arg_2);
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_System_Void_System_Int32_System_Int32_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharNativeEntry/ConvertCharNativeEntry::Method10()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_value = _s0;
		auto* chaos_boxed = new chaos_boxed_type_System_Private_CoreLib_System_Int32{};
		chaos_boxed->header.type_info = &chaos_type_info_System_Private_CoreLib_System_Int32;
		chaos_boxed->value = chaos_value;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Object_System_IFormatProvider_(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_normalize_native_int_argument(chaos_raw_arg_2);
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_System_Void_System_Int32_System_Int32_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharNativeEntry/ConvertCharNativeEntry::Method11()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method11(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_SByte_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_normalize_native_int_argument(chaos_raw_arg_2);
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_System_Void_System_Int32_System_Int32_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharNativeEntry/ConvertCharNativeEntry::Method12()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method12(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = ChaosStoreFloat32(42.0f);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Single_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_normalize_native_int_argument(chaos_raw_arg_2);
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_System_Void_System_Int32_System_Int32_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharNativeEntry/ConvertCharNativeEntry::Method13()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method13(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	{
		auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
		chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
		chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
		chaos_string->utf8_data = "hello";
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
	}
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_String_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_normalize_native_int_argument(chaos_raw_arg_2);
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_System_Void_System_Int32_System_Int32_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharNativeEntry/ConvertCharNativeEntry::Method14()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method14(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	{
		auto* chaos_string = new chaos_type_System_Private_CoreLib_System_String{};
		chaos_string->header.type_info = &chaos_type_info_System_Private_CoreLib_System_String;
		chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(5);
		chaos_string->utf8_data = "hello";
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_raw_arg_1;
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_String_System_IFormatProvider_(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_normalize_native_int_argument(chaos_raw_arg_2);
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_System_Void_System_Int32_System_Int32_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharNativeEntry/ConvertCharNativeEntry::Method15()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method15(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_UInt16_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_normalize_native_int_argument(chaos_raw_arg_2);
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_System_Void_System_Int32_System_Int32_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharNativeEntry/ConvertCharNativeEntry::Method16()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method16(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_UInt32_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_normalize_native_int_argument(chaos_raw_arg_2);
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_System_Void_System_Int32_System_Int32_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharNativeEntry/ConvertCharNativeEntry::Method17()
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Method17(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	CHAOS_IL2CPP_INTPTR _s1{};
	CHAOS_IL2CPP_INTPTR _s2{};


	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_s0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	{
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_raw_arg_0;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_UInt64_(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	_s1 = chaos_locals[0];
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_raw_arg_2 = _s2;
		const auto chaos_arg_2 = chaos_normalize_native_int_argument(chaos_raw_arg_2);
		const auto chaos_raw_arg_1 = _s1;
		const auto chaos_arg_1 = chaos_normalize_native_int_argument(chaos_raw_arg_1);
		const auto chaos_raw_arg_0 = _s0;
		const auto chaos_arg_0 = chaos_normalize_native_int_argument(chaos_raw_arg_0);
		chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_System_Void_System_Int32_System_Int32_System_String_(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: ConvertCharNativeEntry/ConvertCharNativeEntry::Run(System.Int32)
extern "C" CHAOS_IL2CPP_INT32 ConvertCharNativeEntry_ConvertCharNativeEntry_Run(CHAOS_IL2CPP_INT32 chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	CHAOS_IL2CPP_INTPTR _s0{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);

	_s0 = chaos_args[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	chaos_locals[0] = _s0;
	_s0 = chaos_locals[0];
	{
		const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(_s0);
		switch (chaos_switch_value)
		{
			case 0:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method0();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 1:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method1();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 2:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method2();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 3:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method3();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 4:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method4();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 5:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method5();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 6:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method6();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 7:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method7();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 8:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method8();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 9:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method9();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 10:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method10();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 11:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method11();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 12:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method12();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 13:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method13();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 14:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method14();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 15:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method15();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 16:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method16();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			case 17:
			{
				{
					const auto chaos_result = ConvertCharNativeEntry_ConvertCharNativeEntry_Method17();
					_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
				}
				chaos_locals[2] = _s0;
				break;
			}
			default:
			{
				_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
				chaos_locals[2] = _s0;
				break;
			}
		}
	}
	_s0 = chaos_locals[2];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Native AOT entry for ConvertCharNativeEntry/ConvertCharNativeEntry::Run:System.Int32(System.Int32)
// Managed symbol: ConvertCharNativeEntry_ConvertCharNativeEntry_Run
extern "C" CHAOS_IL2CPP_INT32 RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)
{
    return ConvertCharNativeEntry_ConvertCharNativeEntry_Run(chaos_entry_index);
}
#pragma warning(pop)
