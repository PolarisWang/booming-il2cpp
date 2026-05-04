// AUTO-GENERATED from RuntimeHelperShapeRegistry -- DO NOT EDIT
// This file is the single source of truth for runtime helper dispatch.
// All changes must originate in RuntimeHelperShapeRegistry.BuildDefault().

#ifndef CHAOS_IL2CPP_RUNTIME_HELPER_SHAPES_H_
#define CHAOS_IL2CPP_RUNTIME_HELPER_SHAPES_H_

#include <cstdint>
#include <cstddef>

namespace chaos { namespace il2cpp { namespace shape_dispatch {
#pragma pack(push, 1)

// ---- Shape ID enum (FNV-1a hashes of canonical shape keys) ----
enum ShapeId : uint32_t {
    SHAPE_SYSTEM_ACTIVATOR_CREATEINSTANCE_SYSTEM_TYPE = 0x59AC309Eu,
    SHAPE_SYSTEM_ARGUMENTEXCEPTION_GET_PARAMNAME = 0x24D05674u,
    SHAPE_SYSTEM_ARGUMENTOUTOFRANGEEXCEPTION__CTOR_SYSTEM_STRING_SYSTEM_STRING = 0x74FA35FBu,
    SHAPE_SYSTEM_DATETIME_GET_UTCNOW = 0x1254B0E7u,
    SHAPE_SYSTEM_DECIMAL__CTOR_SYSTEM_INT32 = 0xE1CCAB35u,
    SHAPE_SYSTEM_DELEGATE_COMBINE_SYSTEM_DELEGATE_SYSTEM_DELEGATE = 0x85382755u,
    SHAPE_SYSTEM_DELEGATE_REMOVE_SYSTEM_DELEGATE_SYSTEM_DELEGATE = 0x36442080u,
    SHAPE_SYSTEM_DOUBLE_TOSTRING_SYSTEM_STRING = 0x62DBF072u,
    SHAPE_SYSTEM_ENVIRONMENT_GET_CURRENTMANAGEDTHREADID = 0xF4DDECAEu,
    SHAPE_SYSTEM_EXCEPTION_GET_MESSAGE = 0xA28AE25Eu,
    SHAPE_SYSTEM_EXCEPTION__CTOR_SYSTEM_STRING = 0x1B319934u,
    SHAPE_SYSTEM_GC_COLLECT = 0xBC962311u,
    SHAPE_SYSTEM_GC_KEEPALIVE_SYSTEM_OBJECT = 0x4546B70Fu,
    SHAPE_SYSTEM_GC_WAITFORPENDINGFINALIZERS = 0x7E101281u,
    SHAPE_SYSTEM_INT32_TOSTRING = 0xE3BF8C09u,
    SHAPE_SYSTEM_INVALIDOPERATIONEXCEPTION__CTOR_SYSTEM_STRING = 0x637E36D4u,
    SHAPE_SYSTEM_OBJECT_EQUALS_SYSTEM_OBJECT = 0xDD4F3B4Du,
    SHAPE_SYSTEM_OBJECT__CTOR = 0xB8F83EE0u,
    SHAPE_SYSTEM_REFLECTION_ASSEMBLYNAME_GET_NAME = 0xBDE1B28Du,
    SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETNAME = 0xA8FFC0A5u,
    SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETTYPE_SYSTEM_STRING = 0x150F01C4u,
    SHAPE_SYSTEM_REFLECTION_MEMBERINFO_GET_DECLARINGTYPE = 0x8D177D62u,
    SHAPE_SYSTEM_REFLECTION_MEMBERINFO_GET_METADATATOKEN = 0x9AD17BD1u,
    SHAPE_SYSTEM_REFLECTION_MEMBERINFO_GET_NAME = 0x4B1B4F7Cu,
    SHAPE_SYSTEM_REFLECTION_METHODBASE_GETPARAMETERS = 0x19603F22u,
    SHAPE_SYSTEM_REFLECTION_METHODBASE_GET_METHODHANDLE = 0xD04F4056u,
    SHAPE_SYSTEM_REFLECTION_METHODBASE_INVOKE_SYSTEM_OBJECT_SYSTEM_OBJECT__ = 0xB4A9F052u,
    SHAPE_SYSTEM_REFLECTION_METHODINFO_MAKEGENERICMETHOD_SYSTEM_TYPE__ = 0xC85A8048u,
    SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GET_NAME = 0x2C23EC77u,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_DEFAULTINTERPOLATEDSTRINGHANDLER_APPENDFORMATTED_SYSTEM_STRING = 0x73432179u,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_DEFAULTINTERPOLATEDSTRINGHANDLER_APPENDLITERAL_SYSTEM_STRING = 0x7BDDA564u,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_DEFAULTINTERPOLATEDSTRINGHANDLER_TOSTRINGANDCLEAR = 0x0D101A5Bu,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_DEFAULTINTERPOLATEDSTRINGHANDLER__CTOR_SYSTEM_INT32_SYSTEM_INT32 = 0x136B6B0Fu,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_RUNTIMEHELPERS_INITIALIZEARRAY_SYSTEM_ARRAY_SYSTEM_RUNTIMEFIELDHANDLE = 0xC5DD5677u,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_YIELDAWAITABLE_GETAWAITER = 0xC76A5DEEu,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_YIELDAWAITABLE_YIELDAWAITER_GETRESULT = 0x03229253u,
    SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_YIELDAWAITABLE_YIELDAWAITER_GET_ISCOMPLETED = 0x4F469568u,
    SHAPE_SYSTEM_SINGLE_TOSTRING_SYSTEM_STRING = 0x4B5B293Du,
    SHAPE_SYSTEM_STRING_CONCAT_SYSTEM_STRING_SYSTEM_STRING = 0xF6869876u,
    SHAPE_SYSTEM_STRING_CONCAT_SYSTEM_STRING_SYSTEM_STRING_SYSTEM_STRING = 0x08CB997Au,
    SHAPE_SYSTEM_STRING_CONCAT_SYSTEM_STRING_SYSTEM_STRING_SYSTEM_STRING_SYSTEM_STRING = 0xEC14DB7Eu,
    SHAPE_SYSTEM_STRING_CONTAINS_SYSTEM_STRING_SYSTEM_STRINGCOMPARISON = 0x4E63F846u,
    SHAPE_SYSTEM_STRING_GET_LENGTH = 0xA9CB34E1u,
    SHAPE_SYSTEM_STRING_OP_EQUALITY_SYSTEM_STRING_SYSTEM_STRING = 0x591FA874u,
    SHAPE_SYSTEM_STRING_STARTSWITH_SYSTEM_STRING_SYSTEM_STRINGCOMPARISON = 0xCA6DE39Eu,
    SHAPE_SYSTEM_THREADING_MONITOR_ENTER_SYSTEM_OBJECT_SYSTEM_BOOLEAN_ = 0x6E0E6249u,
    SHAPE_SYSTEM_THREADING_MONITOR_EXIT_SYSTEM_OBJECT = 0xAC17C618u,
    SHAPE_SYSTEM_THREADING_TASKS_TASK_YIELD = 0x21E4D84Du,
    SHAPE_SYSTEM_THREADING_THREAD_GET_CURRENTTHREAD = 0x8CE16B91u,
    SHAPE_SYSTEM_THREADING_THREAD_JOIN = 0x411949DFu,
    SHAPE_SYSTEM_THREADING_THREAD_START = 0x4236E9C3u,
    SHAPE_SYSTEM_THREADING_THREAD__CTOR_SYSTEM_THREADING_THREADSTART = 0xCCAB9078u,
    SHAPE_SYSTEM_TIMESPAN_FROMMILLISECONDS_SYSTEM_DOUBLE = 0x41A528C8u,
    SHAPE_SYSTEM_TYPE_GETCONSTRUCTORS_SYSTEM_REFLECTION_BINDINGFLAGS = 0x731944A2u,
    SHAPE_SYSTEM_TYPE_GETFIELD_SYSTEM_STRING = 0xCABE899Du,
    SHAPE_SYSTEM_TYPE_GETGENERICARGUMENTS = 0xCDF68F9Au,
    SHAPE_SYSTEM_TYPE_GETGENERICTYPEDEFINITION = 0xE5F1AB95u,
    SHAPE_SYSTEM_TYPE_GETMETHOD_SYSTEM_STRING = 0x4867F576u,
    SHAPE_SYSTEM_TYPE_GETMETHOD_SYSTEM_STRING_SYSTEM_REFLECTION_BINDINGFLAGS = 0x0D98C752u,
    SHAPE_SYSTEM_TYPE_GETTYPEFROMHANDLE_SYSTEM_RUNTIMETYPEHANDLE = 0xEBD6B558u,
    SHAPE_SYSTEM_TYPE_GETTYPE_SYSTEM_STRING = 0x2BB034B1u,
    SHAPE_SYSTEM_TYPE_GET_ASSEMBLY = 0x3634727Cu,
    SHAPE_SYSTEM_TYPE_GET_TYPEHANDLE = 0xA5CEC5FAu,

    SHAPE_COUNT = 63u,
};

// ---- Compile-time dispatch: NativeInt-returning shapes ----
template <ShapeId S, typename... Args>
CHAOS_IL2CPP_INTPTR DispatchNativeInt(Args... args) {
    if constexpr (S == SHAPE_SYSTEM_ACTIVATOR_CREATEINSTANCE_SYSTEM_TYPE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_create_instance(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_ARGUMENTEXCEPTION_GET_PARAMNAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_exception_param_name(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_DATETIME_GET_UTCNOW) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_external_runtime_System_Private_CoreLib_System_DateTime__get_UtcNow_System_DateTime__(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_DELEGATE_COMBINE_SYSTEM_DELEGATE_SYSTEM_DELEGATE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_delegate_combine(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_DELEGATE_REMOVE_SYSTEM_DELEGATE_SYSTEM_DELEGATE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_delegate_remove(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_DOUBLE_TOSTRING_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_format_double_to_string(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_EXCEPTION_GET_MESSAGE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_exception_message(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_INT32_TOSTRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_format_int32_to_string(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_OBJECT_EQUALS_SYSTEM_OBJECT) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_object_equals(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_ASSEMBLYNAME_GET_NAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_assembly_name_value(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETNAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_assembly_name(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_ASSEMBLY_GETTYPE_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_type_from_assembly(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_MEMBERINFO_GET_DECLARINGTYPE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_declaring_type(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_MEMBERINFO_GET_NAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_member_name(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_METHODBASE_GETPARAMETERS) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_parameters(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_METHODBASE_GET_METHODHANDLE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_method_handle(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_METHODBASE_INVOKE_SYSTEM_OBJECT_SYSTEM_OBJECT__) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_invoke_method(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_METHODINFO_MAKEGENERICMETHOD_SYSTEM_TYPE__) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_make_generic_method(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_PARAMETERINFO_GET_NAME) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_parameter_name(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_DEFAULTINTERPOLATEDSTRINGHANDLER_TOSTRINGANDCLEAR) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_default_interpolated_string_handler_to_string_and_clear(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_YIELDAWAITABLE_GETAWAITER) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_async_yield_get_awaiter(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_YIELDAWAITABLE_YIELDAWAITER_GET_ISCOMPLETED) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_async_yield_get_is_completed(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_SINGLE_TOSTRING_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_format_single_to_string(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_STRING_CONCAT_SYSTEM_STRING_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_concat_string_pair_values(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_STRING_CONTAINS_SYSTEM_STRING_SYSTEM_STRINGCOMPARISON) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_string_contains(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_STRING_OP_EQUALITY_SYSTEM_STRING_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_object_equals(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_STRING_STARTSWITH_SYSTEM_STRING_SYSTEM_STRINGCOMPARISON) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_string_starts_with(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_THREADING_TASKS_TASK_YIELD) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_async_yield_create(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_THREADING_THREAD_GET_CURRENTTHREAD) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_thread_get_current(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TIMESPAN_FROMMILLISECONDS_SYSTEM_DOUBLE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos::il2cpp::support::TimeSpanFromMilliseconds(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETCONSTRUCTORS_SYSTEM_REFLECTION_BINDINGFLAGS) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_constructors(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETFIELD_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_field(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETGENERICARGUMENTS) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_generic_arguments(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETGENERICTYPEDEFINITION) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_generic_type_definition(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETMETHOD_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_method(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETMETHOD_SYSTEM_STRING_SYSTEM_REFLECTION_BINDINGFLAGS) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_method(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETTYPEFROMHANDLE_SYSTEM_RUNTIMETYPEHANDLE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_type_from_handle(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GETTYPE_SYSTEM_STRING) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_type_by_name(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_ASSEMBLY) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_assembly(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_TYPE_GET_TYPEHANDLE) {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
            chaos_reflection_get_type_handle(args...));
    }
    else {
        static_assert(S != S, "Unhandled shape ID in DispatchNativeInt");
        return 0;
    }
}

// ---- Compile-time dispatch: Void-returning shapes ----
template <ShapeId S, typename... Args>
void DispatchVoid(Args... args) {
    else if constexpr (S == SHAPE_SYSTEM_ARGUMENTOUTOFRANGEEXCEPTION__CTOR_SYSTEM_STRING_SYSTEM_STRING) {
        chaos_reflection_set_exception_metadata(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_DECIMAL__CTOR_SYSTEM_INT32) {
        chaos_decimal_ctor_int32(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_EXCEPTION__CTOR_SYSTEM_STRING) {
        chaos_reflection_set_exception_metadata(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_GC_KEEPALIVE_SYSTEM_OBJECT) {
        chaos_gc_keepalive(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_INVALIDOPERATIONEXCEPTION__CTOR_SYSTEM_STRING) {
        chaos_reflection_set_exception_metadata(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_OBJECT__CTOR) {
        chaos_object_ctor(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_DEFAULTINTERPOLATEDSTRINGHANDLER_APPENDFORMATTED_SYSTEM_STRING) {
        chaos_default_interpolated_string_handler_append_string(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_DEFAULTINTERPOLATEDSTRINGHANDLER_APPENDLITERAL_SYSTEM_STRING) {
        chaos_default_interpolated_string_handler_append_string(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_DEFAULTINTERPOLATEDSTRINGHANDLER__CTOR_SYSTEM_INT32_SYSTEM_INT32) {
        chaos_default_interpolated_string_handler_reset(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_RUNTIMEHELPERS_INITIALIZEARRAY_SYSTEM_ARRAY_SYSTEM_RUNTIMEFIELDHANDLE) {
        chaos_initialize_array_from_field_data_int32(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_RUNTIME_COMPILERSERVICES_YIELDAWAITABLE_YIELDAWAITER_GETRESULT) {
        chaos_async_yield_get_result(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_THREADING_MONITOR_ENTER_SYSTEM_OBJECT_SYSTEM_BOOLEAN_) {
        chaos_monitor_enter(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_THREADING_MONITOR_EXIT_SYSTEM_OBJECT) {
        chaos_monitor_exit(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_THREADING_THREAD_JOIN) {
        chaos_thread_join(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_THREADING_THREAD_START) {
        chaos_thread_start(args...);
    }
    else if constexpr (S == SHAPE_SYSTEM_THREADING_THREAD__CTOR_SYSTEM_THREADING_THREADSTART) {
        chaos_thread_ctor(args...);
    }
    else {
        static_assert(S != S, "Unhandled shape ID in DispatchVoid");
    }
}

// ---- Compile-time dispatch: Int32-returning shapes ----
template <ShapeId S, typename... Args>
CHAOS_IL2CPP_INT32 DispatchInt32(Args... args) {
    else if constexpr (S == SHAPE_SYSTEM_ENVIRONMENT_GET_CURRENTMANAGEDTHREADID) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            chaos_current_managed_thread_id(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_REFLECTION_MEMBERINFO_GET_METADATATOKEN) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            chaos_reflection_get_metadata_token(args...));
    }
    else if constexpr (S == SHAPE_SYSTEM_STRING_GET_LENGTH) {
        return static_cast<CHAOS_IL2CPP_INT32>(
            chaos_string_get_length(args...));
    }
    else {
        static_assert(S != S, "Unhandled shape ID in DispatchInt32");
        return 0;
    }
}

// ---- Runtime dispatch table (HotUpdate registration) ----
using ShapeRuntimeDispatchFn = void* (*)(void** args, uint32_t argc);

struct ShapeRuntimeEntry {
    ShapeId id;
    ShapeRuntimeDispatchFn fn;
};

constexpr uint32_t kMaxRuntimeShapeEntries = 32;
extern ShapeRuntimeEntry g_runtime_shape_entries[kMaxRuntimeShapeEntries];
extern uint32_t g_runtime_shape_count;

// ---- Compile-time completeness verification ----
static_assert(SHAPE_COUNT == 63u,
    "Number of registered shapes changed. Regenerate this header from RuntimeHelperShapeRegistry.");

#pragma pack(pop)
}}}  // namespace chaos::il2cpp::shape_dispatch

#endif // CHAOS_IL2CPP_RUNTIME_HELPER_SHAPES_H_