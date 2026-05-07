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
	(void)chaos_fn_arg_2;
	if (chaos_fn_arg_0 == chaos_fn_arg_1)
	{
		return;
	}

chaos_static_Chaos_TestFramework_Sdk_Chaos_TestFramework_ChaosAssertState__ExitCode = static_cast<CHAOS_IL2CPP_INTPTR>(1);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Boolean_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_convert_tochar_boolean(chaos_fn_arg_0);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Byte_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_convert_tochar_byte(chaos_fn_arg_0);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Char_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_convert_tochar_char(chaos_fn_arg_0);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_DateTime_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_convert_tochar_datetime(chaos_fn_arg_0);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Decimal_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_convert_tochar_decimal(chaos_fn_arg_0);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Double_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_convert_tochar_double(chaos_fn_arg_0);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Int16_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_convert_tochar_int16(chaos_fn_arg_0);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Int32_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_convert_tochar_int32(chaos_fn_arg_0);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Int64_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_convert_tochar_int64(chaos_fn_arg_0);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Object_System_IFormatProvider_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	return chaos_convert_tochar_object_provider(chaos_fn_arg_0, chaos_fn_arg_1);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_SByte_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_convert_tochar_sbyte(chaos_fn_arg_0);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Single_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_convert_tochar_single(chaos_fn_arg_0);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_String_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_convert_tochar_string(chaos_fn_arg_0);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_String_System_IFormatProvider_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	return chaos_convert_tochar_string_provider(chaos_fn_arg_0, chaos_fn_arg_1);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_UInt16_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_convert_tochar_uint16(chaos_fn_arg_0);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_UInt32_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_convert_tochar_uint32(chaos_fn_arg_0);
}

extern "C" inline CHAOS_IL2CPP_UINT16 chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_UInt64_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	return chaos_convert_tochar_uint64(chaos_fn_arg_0);
}

extern "C" inline CHAOS_IL2CPP_INT64 chaos_external_runtime_System_Private_CoreLib_System_DateTime__get_UtcNow_System_DateTime__(void)

#pragma warning(pop)
extern "C" int __chaos_assert_failures = 0;
extern "C" int RunNativeAot(int i) { return 0; }
