// Auto-generated type declarations (TU paging)
#pragma once

#include <chaos/native_types.h>
#include <chaos/type_info.h>  // MethodTable, TypeInfoV0 (complete type)
#include "generated_code_compat.h"  // PureTypeHeader for delegate type definitions
#include "ChaosGeneratedRuntimePrelude.h"  // chaos_managed_array for array-compat checks

typedef CHAOS_IL2CPP_INT32 chaos_valuetype_System_Int32;

struct chaos_type_SnapshotTestFixtures_Helper

{

    ThinLockableHeader header{};

};



struct chaos_type_SnapshotTestFixtures_MathHelper

{

    ThinLockableHeader header{};

};



struct chaos_type_System_Private_CoreLib_System_Object

{

    ThinLockableHeader header{};

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




struct chaos_type_SnapshotTestFixtures_Helper;
struct chaos_boxed_type_SnapshotTestFixtures_Helper;
struct chaos_type_SnapshotTestFixtures_Helper__Square_System_Int32_System_Int32_;
struct chaos_boxed_type_SnapshotTestFixtures_Helper__Square_System_Int32_System_Int32_;
struct chaos_type_SnapshotTestFixtures_MathHelper;
struct chaos_boxed_type_SnapshotTestFixtures_MathHelper;
struct chaos_type_System_Int32;
struct chaos_boxed_type_System_Int32;
struct chaos_type_System_Private_CoreLib_System_Action {
    PureTypeHeader header{};
    CHAOS_IL2CPP_INTPTR chaos_delegate_target = 0;
    CHAOS_IL2CPP_INTPTR chaos_delegate_method_ptr = 0;
    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_list = 0;
    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_count = 0;
    CHAOS_IL2CPP_UINT32 chaos_delegate_method_token = 0;
    CHAOS_IL2CPP_UINT32 _pad = 0;
};
struct chaos_boxed_type_System_Private_CoreLib_System_Action;
struct chaos_type_System_Private_CoreLib_System_Delegate;
struct chaos_boxed_type_System_Private_CoreLib_System_Delegate;
struct chaos_type_System_Private_CoreLib_System_MulticastDelegate;
struct chaos_boxed_type_System_Private_CoreLib_System_MulticastDelegate;
struct chaos_type_System_Private_CoreLib_System_Object;
struct chaos_boxed_type_System_Private_CoreLib_System_Object;
struct chaos_type_System_Private_CoreLib_System_Reflection_Assembly;
struct chaos_boxed_type_System_Private_CoreLib_System_Reflection_Assembly;
struct chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName;
struct chaos_boxed_type_System_Private_CoreLib_System_Reflection_AssemblyName;
struct chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo;
struct chaos_boxed_type_System_Private_CoreLib_System_Reflection_ConstructorInfo;
struct chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo;
struct chaos_boxed_type_System_Private_CoreLib_System_Reflection_FieldInfo;
struct chaos_type_System_Private_CoreLib_System_Reflection_MethodBase;
struct chaos_boxed_type_System_Private_CoreLib_System_Reflection_MethodBase;
struct chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo;
struct chaos_boxed_type_System_Private_CoreLib_System_Reflection_MethodInfo;
struct chaos_type_System_Private_CoreLib_System_String;
struct chaos_boxed_type_System_Private_CoreLib_System_String;
struct chaos_type_System_Private_CoreLib_System_Type;
struct chaos_boxed_type_System_Private_CoreLib_System_Type;

struct HotpatchEntryV0;
extern "C" HotpatchEntryV0 s_hotpatch_entries[];

namespace chaos::il2cpp::codegen::SnapshotTestFixtures
{
extern MethodTable chaos_mt_SnapshotTestFixtures_Helper;
extern MethodTable chaos_mt_SnapshotTestFixtures_Helper__Square_System_Int32_System_Int32_;
extern MethodTable chaos_mt_SnapshotTestFixtures_MathHelper;
extern MethodTable chaos_mt_System_Int32;
extern MethodTable chaos_mt_System_Private_CoreLib_System_Action;
extern MethodTable chaos_mt_System_Private_CoreLib_System_Delegate;
extern MethodTable chaos_mt_System_Private_CoreLib_System_MulticastDelegate;
extern MethodTable chaos_mt_System_Private_CoreLib_System_Object;
extern MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_Assembly;
extern MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName;
extern MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo;
extern MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo;
extern MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase;
extern MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo;
extern MethodTable chaos_mt_System_Private_CoreLib_System_String;
extern MethodTable chaos_mt_System_Private_CoreLib_System_Type;


extern CHAOS_IL2CPP_INTPTR chaos_static_SnapshotTestFixtures_Helper__Square_System_Int32_System_Int32_;

extern const CHAOS_IL2CPP_UINT64 chaos_type_id_SnapshotTestFixtures_Helper;
extern const CHAOS_IL2CPP_UINT64 chaos_type_id_SnapshotTestFixtures_Helper__Square_System_Int32_System_Int32_;
extern const CHAOS_IL2CPP_UINT64 chaos_type_id_SnapshotTestFixtures_MathHelper;
extern const CHAOS_IL2CPP_UINT64 chaos_type_id_System_Int32;
extern const CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Action;
extern const CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Delegate;
extern const CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_MulticastDelegate;
extern const CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Object;
extern const CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly;
extern const CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName;
extern const CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo;
extern const CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo;
extern const CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_MethodBase;
extern const CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo;
extern const CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_String;
extern const CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Type;

extern "C" void chaos_ensure_type_initialized_SnapshotTestFixtures_MathHelper(void);
extern "C" void chaos_ensure_type_initialized_System_Int32(void);
extern "C" void chaos_ensure_type_initialized_SnapshotTestFixtures_Helper(void);
extern "C" void chaos_ensure_type_initialized_System_Private_CoreLib_System_String(void);
extern "C" void chaos_ensure_type_initialized_System_Private_CoreLib_System_Action(void);
extern "C" void chaos_ensure_type_initialized_System_Private_CoreLib_System_MulticastDelegate(void);
extern "C" void chaos_ensure_type_initialized_System_Private_CoreLib_System_Delegate(void);
extern "C" void chaos_ensure_type_initialized_System_Private_CoreLib_System_Object(void);
extern "C" void chaos_ensure_type_initialized_System_Private_CoreLib_System_Type(void);
extern "C" void chaos_ensure_type_initialized_System_Private_CoreLib_System_Reflection_MethodBase(void);
extern "C" void chaos_ensure_type_initialized_System_Private_CoreLib_System_Reflection_MethodInfo(void);
extern "C" void chaos_ensure_type_initialized_System_Private_CoreLib_System_Reflection_ConstructorInfo(void);
extern "C" void chaos_ensure_type_initialized_System_Private_CoreLib_System_Reflection_FieldInfo(void);
extern "C" void chaos_ensure_type_initialized_System_Private_CoreLib_System_Reflection_Assembly(void);
extern "C" void chaos_ensure_type_initialized_System_Private_CoreLib_System_Reflection_AssemblyName(void);
extern "C" void chaos_ensure_type_initialized_SnapshotTestFixtures_Helper__Square_System_Int32_System_Int32_(void);

CHAOS_IL2CPP_INTPTR chaos_string_materialize(CHAOS_IL2CPP_INTPTR chaos_value) noexcept;

bool chaos_is_array_store_compatible(const chaos_managed_array* chaos_array, CHAOS_IL2CPP_INTPTR chaos_value) noexcept;

void chaos_default_interpolated_string_handler_reset(CHAOS_IL2CPP_INTPTR chaos_handler_ref, CHAOS_IL2CPP_INT32 chaos_literal_length, CHAOS_IL2CPP_INT32 chaos_trailing_count);
void chaos_default_interpolated_string_handler_append_string(CHAOS_IL2CPP_INTPTR chaos_handler_ref, CHAOS_IL2CPP_INTPTR chaos_string_value);
void chaos_default_interpolated_string_handler_append_int32(CHAOS_IL2CPP_INTPTR chaos_handler_ref, CHAOS_IL2CPP_INT32 chaos_value);
CHAOS_IL2CPP_INTPTR chaos_default_interpolated_string_handler_to_string_and_clear(CHAOS_IL2CPP_INTPTR chaos_handler_ref);

} // namespace chaos::il2cpp::codegen::SnapshotTestFixtures

extern "C" void* kChaosExternalRuntimeFnTable[];

extern "C" CHAOS_IL2CPP_INT32 kChaosExternalRuntimeCount;
extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_SnapshotTestFixtures_Helper__Square_System_Int32_System_Int32_(CHAOS_IL2CPP_INTPTR) noexcept;

void ChaosReflectionSetExceptionMetadata_2params(CHAOS_IL2CPP_INTPTR chaos_exception, CHAOS_IL2CPP_INTPTR chaos_message, CHAOS_IL2CPP_INTPTR chaos_param_name);


// chaos_valuetype_* typedefs (safety net: TPG stub declarations)
typedef CHAOS_IL2CPP_INT32 chaos_valuetype_System_Int32;

