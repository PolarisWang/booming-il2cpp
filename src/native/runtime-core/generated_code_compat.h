#ifndef CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_
#define CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_

// ── Runtime API declarations exposed to generated code ──
//
// This header provides declarations used by generated .cpp files via
// runtime_core.h. After the Hybrid TypeInfo* migration, all 28 passing
// families generate self-contained C++ code.

#include <chaos/native_types.h>
#include <chaos/type_info.h>

// ── Object header definition ──
// type_info: type identity for GC, casting, reflection
// vtable: virtual method dispatch table
struct chaos_object_header {
    const void**    vtable      = nullptr;  // [0] virtual method table
    const TypeInfo* type_info   = nullptr;   // [8] type identity
};

// ── Managed string type ──────────────────────────────────────────
// Used by generated code for reinterpret_cast access to string length.
struct chaos_managed_string {
    chaos_object_header header{};
    CHAOS_IL2CPP_INT32 length = 0;
};

#define CHAOS_IL2CPP_STRING_TYPE chaos_managed_string

// ── Reflection struct type definitions ────────────────────────────
// Used by generated code for reflection object marshalling.
// The codegen emits extern runtime functions that work with these
// struct layouts; the definitions must stay in sync.

struct chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo {
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
    CHAOS_IL2CPP_INTPTR runtime_method_handle = 0;
    CHAOS_IL2CPP_INTPTR generic_definition_method_handle = 0;
    CHAOS_IL2CPP_INTPTR generic_argument_type_handle = 0;
    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
    CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo {
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
    CHAOS_IL2CPP_INTPTR runtime_method_handle = 0;
    CHAOS_IL2CPP_INTPTR generic_definition_method_handle = 0;
    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
    CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo {
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
    CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_Assembly {
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR runtime_assembly_handle = 0;
    CHAOS_IL2CPP_INTPTR runtime_assembly_name_value = 0;
};

struct chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName {
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;
};

// ── Exception metadata functions (extern C declarations) ──
// Implementations live in exception_api.cpp.
extern "C" {
void chaos_reflection_set_exception_metadata(CHAOS_IL2CPP_INTPTR exception_obj, CHAOS_IL2CPP_INTPTR message_value);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_exception_message(CHAOS_IL2CPP_INTPTR exception_obj);
}  // extern "C"

// ── Runtime stub declarations ───────────────────────────────────
// Stub implementations in runtime_stubs.cpp. These are thin wrappers
// called by generated extern "C" external-runtime-helper functions.
extern "C" {
// Array
void    chaos_array_clear(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 count) noexcept;
CHAOS_IL2CPP_INT32 chaos_array_get_length(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 dimension) noexcept;
// Type marshalling
CHAOS_IL2CPP_INTPTR chaos_store_int64(CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INTPTR chaos_store_float64(CHAOS_IL2CPP_FLOAT64 value) noexcept;
CHAOS_IL2CPP_INT64  chaos_load_int64(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_FLOAT64 chaos_load_float64(CHAOS_IL2CPP_INTPTR value) noexcept;
// Buffer
CHAOS_IL2CPP_INT32  chaos_buffer_byte_length(CHAOS_IL2CPP_INTPTR array) noexcept;
// DateTime
CHAOS_IL2CPP_INT64  chaos_datetime_get_utc_now(void) noexcept;
CHAOS_IL2CPP_INT32  chaos_datetime_get_hash_code(CHAOS_IL2CPP_INTPTR datetime) noexcept;
// Math
CHAOS_IL2CPP_INT64  chaos_math_sqrt(CHAOS_IL2CPP_INT64 value) noexcept;
// Threading
void    chaos_interlocked_memory_barrier(void) noexcept;
// Exception
CHAOS_IL2CPP_INTPTR chaos_exception_get_base_exception(CHAOS_IL2CPP_INTPTR exc) noexcept;
CHAOS_IL2CPP_INTPTR chaos_exception_get_inner_exception(CHAOS_IL2CPP_INTPTR exc) noexcept;
CHAOS_IL2CPP_INT32  chaos_exception_get_hresult(CHAOS_IL2CPP_INTPTR exc) noexcept;
// Object
CHAOS_IL2CPP_INTPTR chaos_object_equals_static(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) noexcept;
void    chaos_object_ctor(CHAOS_IL2CPP_INTPTR obj) noexcept;
// GUID / Random
CHAOS_IL2CPP_INTPTR chaos_guid_new_guid(void) noexcept;
void    chaos_random_next_bytes(CHAOS_IL2CPP_INTPTR rng, CHAOS_IL2CPP_INTPTR buffer) noexcept;
CHAOS_IL2CPP_INT64  chaos_random_next_double(CHAOS_IL2CPP_INTPTR rng) noexcept;
// Culture
CHAOS_IL2CPP_INTPTR chaos_culture_get_current(void) noexcept;
CHAOS_IL2CPP_INTPTR chaos_culture_get_invariant(void) noexcept;
CHAOS_IL2CPP_INTPTR chaos_culture_get_compare_info(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR chaos_culture_get_date_time_format(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR chaos_culture_get_display_name(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR chaos_culture_get_name(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR chaos_culture_get_number_format(CHAOS_IL2CPP_INTPTR culture) noexcept;
// String
CHAOS_IL2CPP_INTPTR chaos_string_contains(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR chaos_string_join_ss(CHAOS_IL2CPP_INTPTR separator, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR chaos_string_starts_with(CHAOS_IL2CPP_INTPTR str, CHAOS_IL2CPP_INTPTR value) noexcept;
// Reflection
CHAOS_IL2CPP_INTPTR chaos_reflection_is_defined(CHAOS_IL2CPP_INTPTR assembly, CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_is_subclass_of(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR base) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_generic_type(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_value_type(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_namespace(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_reflected_type(CHAOS_IL2CPP_INTPTR member) noexcept;
CHAOS_IL2CPP_INT32  chaos_reflection_get_calling_convention(CHAOS_IL2CPP_INTPTR method) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_public(CHAOS_IL2CPP_INTPTR member) noexcept;
// Module reflection
CHAOS_IL2CPP_INTPTR chaos_reflection_module_get_type(CHAOS_IL2CPP_INTPTR module, CHAOS_IL2CPP_INTPTR name) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_module_get_types(CHAOS_IL2CPP_INTPTR module) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_module_assembly(CHAOS_IL2CPP_INTPTR module) noexcept;
// Parameter reflection
CHAOS_IL2CPP_INTPTR chaos_reflection_get_required_custom_modifiers(CHAOS_IL2CPP_INTPTR param) noexcept;
CHAOS_IL2CPP_INT32  chaos_reflection_get_param_attributes(CHAOS_IL2CPP_INTPTR param) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_default_value(CHAOS_IL2CPP_INTPTR param) noexcept;
// Reflection image/constructed-generic
CHAOS_IL2CPP_INTPTR chaos_reflection_get_image_runtime_version(CHAOS_IL2CPP_INTPTR assembly) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_constructed_generic(CHAOS_IL2CPP_INTPTR type) noexcept;
// Runtime helpers
CHAOS_IL2CPP_INTPTR chaos_formattablestring_factory_create(CHAOS_IL2CPP_INTPTR format, CHAOS_IL2CPP_INTPTR args) noexcept;
CHAOS_IL2CPP_INTPTR chaos_runtime_helpers_equals(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) noexcept;
CHAOS_IL2CPP_INT32  chaos_runtime_helpers_get_hash_code(CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR chaos_runtime_helpers_get_object_value(CHAOS_IL2CPP_INTPTR value) noexcept;
// Runtime method handle
CHAOS_IL2CPP_INT32  chaos_runtimemethodhandle_get_hash_code(CHAOS_IL2CPP_INT64 handle) noexcept;
// Runtime-wrapped exception
CHAOS_IL2CPP_INTPTR chaos_runtimewrapped_get_wrapped_exception(CHAOS_IL2CPP_INTPTR exc) noexcept;
// Volatile
CHAOS_IL2CPP_INT32  chaos_volatile_read(CHAOS_IL2CPP_INTPTR ptr) noexcept;
	// Reflection supplementary
	CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_sealed(CHAOS_IL2CPP_INTPTR type) noexcept;
	CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_generic_type_def(CHAOS_IL2CPP_INTPTR type) noexcept;
	CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_from_assembly_bool(CHAOS_IL2CPP_INTPTR assembly, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INT32 throw_on_error) noexcept;
	CHAOS_IL2CPP_INTPTR chaos_reflection_get_executing_assembly(void) noexcept;
	CHAOS_IL2CPP_INTPTR chaos_reflection_get_assembly_location(CHAOS_IL2CPP_INTPTR assembly) noexcept;
// Float32 marshalling
CHAOS_IL2CPP_INTPTR chaos_store_float32(CHAOS_IL2CPP_FLOAT32 value) noexcept;
// Reflection assignable
CHAOS_IL2CPP_INTPTR chaos_reflection_is_assignable_from(CHAOS_IL2CPP_INTPTR target, CHAOS_IL2CPP_INTPTR source) noexcept;
// Reflection generic
CHAOS_IL2CPP_INTPTR chaos_reflection_get_generic_param_constraints(CHAOS_IL2CPP_INTPTR type) noexcept;
// Reflection member info
CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_virtual(CHAOS_IL2CPP_INTPTR member) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_interface(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_array(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_assembly_full_name(CHAOS_IL2CPP_INTPTR assembly) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_generic_param_pos(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_enum(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_abstract(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_assembly_qualified_name(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_assembly_get_types(CHAOS_IL2CPP_INTPTR assembly) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_base_definition(CHAOS_IL2CPP_INTPTR member) noexcept;
// Reflection field handle
CHAOS_IL2CPP_INT32  chaos_runtimefieldhandle_get_hash_code(CHAOS_IL2CPP_INTPTR handle) noexcept;
// Reflection module
CHAOS_IL2CPP_INTPTR chaos_reflection_get_module_name(CHAOS_IL2CPP_INTPTR module) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_module_name_only(CHAOS_IL2CPP_INTPTR module) noexcept;
// Parameter reflection (supplementary)
CHAOS_IL2CPP_INTPTR chaos_reflection_has_default_value(CHAOS_IL2CPP_INTPTR param) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_parameter_type(CHAOS_IL2CPP_INTPTR param) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_param_position(CHAOS_IL2CPP_INTPTR param) noexcept;
// Reflection type handle
CHAOS_IL2CPP_INT32  chaos_runtimetypehandle_get_hash_code(CHAOS_IL2CPP_INTPTR handle) noexcept;
// Reflection constructors
CHAOS_IL2CPP_INTPTR chaos_reflection_get_constructors_default(CHAOS_IL2CPP_INTPTR type) noexcept;
// Reflection assignable (supplementary)
CHAOS_IL2CPP_INTPTR chaos_reflection_is_assignable_to(CHAOS_IL2CPP_INTPTR target, CHAOS_IL2CPP_INTPTR source) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_is_instance_of_type(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INTPTR type) noexcept;
// Assembly reflection
CHAOS_IL2CPP_INTPTR chaos_reflection_get_calling_assembly(void) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_entry_assembly(void) noexcept;
// Generic type reflection
CHAOS_IL2CPP_INTPTR chaos_reflection_make_generic_type(CHAOS_IL2CPP_INTPTR def, CHAOS_IL2CPP_INTPTR args) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_contains_generic_params(CHAOS_IL2CPP_INTPTR type) noexcept;
// Parameter default value
CHAOS_IL2CPP_INTPTR chaos_reflection_get_raw_default_value(CHAOS_IL2CPP_INTPTR param) noexcept;
// Type reflection
CHAOS_IL2CPP_INTPTR chaos_reflection_get_fields_bindingflags(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT32 flags) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_methods_bindingflags(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INT32 flags) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_base_type(CHAOS_IL2CPP_INTPTR type) noexcept;
CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_full_name(CHAOS_IL2CPP_INTPTR type) noexcept;
}  // extern "C"

#endif  // CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_
