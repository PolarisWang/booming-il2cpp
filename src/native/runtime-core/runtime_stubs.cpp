// runtime_stubs.cpp — Pre-existing runtime helper stubs
//
// These functions are referenced by generated C++ code but have no real
// runtime implementation yet. All stubs return 0 / nullptr / no-op so
// that native compilation succeeds. Replace with real implementations
// as the corresponding managed features are brought online.
//
// See generated_code_compat.h for the extern "C" declarations.

#include <chaos/common.h>
#include <cstring>

extern "C"
{

// ── Array operations ───────────────────────────────────────────────

void chaos_array_clear(CHAOS_IL2CPP_INTPTR /*array*/, CHAOS_IL2CPP_INT32 /*index*/, CHAOS_IL2CPP_INT32 /*count*/) noexcept
{
}

CHAOS_IL2CPP_INT32 chaos_array_get_length(CHAOS_IL2CPP_INTPTR /*array*/, CHAOS_IL2CPP_INT32 /*dimension*/) noexcept
{
    return 0;
}

// ── Type marshalling helpers ───────────────────────────────────────
// The eval stack stores values as CHAOS_IL2CPP_INTPTR. These helpers
// pack/unpack 64-bit integer and double values. On x64, CHAOS_IL2CPP_INTPTR
// is already 64 bits so store/load_int64 are identity operations.

CHAOS_IL2CPP_INTPTR chaos_store_int64(CHAOS_IL2CPP_INT64 value) noexcept
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(value);
}

CHAOS_IL2CPP_INTPTR chaos_store_float64(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    CHAOS_IL2CPP_INT64 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return static_cast<CHAOS_IL2CPP_INTPTR>(bits);
}

CHAOS_IL2CPP_INT64 chaos_load_int64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT64>(value);
}

CHAOS_IL2CPP_FLOAT64 chaos_load_float64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    CHAOS_IL2CPP_INT64 bits = static_cast<CHAOS_IL2CPP_INT64>(value);
    CHAOS_IL2CPP_FLOAT64 result;
    std::memcpy(&result, &bits, sizeof(result));
    return result;
}

// ─── Buffer operations ─────────────────────────────────────────────

CHAOS_IL2CPP_INT32 chaos_buffer_byte_length(CHAOS_IL2CPP_INTPTR /*array*/) noexcept
{
    return 0;
}

// ─── DateTime operations ──────────────────────────────────────────

CHAOS_IL2CPP_INT64 chaos_datetime_get_utc_now(void) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INT32 chaos_datetime_get_hash_code(CHAOS_IL2CPP_INTPTR /*datetime*/) noexcept
{
    return 0;
}

// ─── Math operations ──────────────────────────────────────────────

CHAOS_IL2CPP_INT64 chaos_math_sqrt(CHAOS_IL2CPP_INT64 value) noexcept
{
    double d;
    CHAOS_IL2CPP_MEMCPY(&d, &value, sizeof(d));
    d = CHAOS_IL2CPP_SQRT(d);
    CHAOS_IL2CPP_INT64 result;
    CHAOS_IL2CPP_MEMCPY(&result, &d, sizeof(result));
    return result;
}

// ─── Interlocked / threading ──────────────────────────────────────

void chaos_interlocked_memory_barrier(void) noexcept
{
}

// ─── Exception helpers ────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_exception_get_base_exception(CHAOS_IL2CPP_INTPTR exc) noexcept
{
    return exc;
}

CHAOS_IL2CPP_INTPTR chaos_exception_get_inner_exception(CHAOS_IL2CPP_INTPTR exc) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INT32 chaos_exception_get_hresult(CHAOS_IL2CPP_INTPTR /*exc*/) noexcept
{
    return 0;
}

// ─── Object helpers ───────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_object_equals_static(CHAOS_IL2CPP_INTPTR /*left*/, CHAOS_IL2CPP_INTPTR /*right*/) noexcept
{
    return 0;
}

void chaos_object_ctor(CHAOS_IL2CPP_INTPTR /*obj*/) noexcept
{
}

// ─── GUID / Random ────────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_guid_new_guid(void) noexcept
{
    return 0;
}

void chaos_random_next_bytes(CHAOS_IL2CPP_INTPTR /*rng*/, CHAOS_IL2CPP_INTPTR /*buffer*/) noexcept
{
}

CHAOS_IL2CPP_INT64 chaos_random_next_double(CHAOS_IL2CPP_INTPTR /*rng*/) noexcept
{
    return 0;
}

// ─── Culture helpers ──────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_culture_get_current(void) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_culture_get_invariant(void) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_culture_get_compare_info(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_culture_get_date_time_format(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_culture_get_display_name(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_culture_get_name(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_culture_get_number_format(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return 0;
}

// ─── String helpers ───────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_string_contains(CHAOS_IL2CPP_INTPTR /*str*/, CHAOS_IL2CPP_INTPTR /*value*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_string_join_ss(CHAOS_IL2CPP_INTPTR /*separator*/, CHAOS_IL2CPP_INTPTR /*value*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_string_starts_with(CHAOS_IL2CPP_INTPTR /*str*/, CHAOS_IL2CPP_INTPTR /*value*/) noexcept
{
    return 0;
}

// ─── Reflection helpers ───────────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_reflection_is_defined(CHAOS_IL2CPP_INTPTR /*assembly*/, CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_is_subclass_of(CHAOS_IL2CPP_INTPTR /*type*/, CHAOS_IL2CPP_INTPTR /*base*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_generic_type(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_value_type(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_namespace(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_reflected_type(CHAOS_IL2CPP_INTPTR /*member*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INT32 chaos_reflection_get_calling_convention(CHAOS_IL2CPP_INTPTR /*method*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_public(CHAOS_IL2CPP_INTPTR /*member*/) noexcept
{
    return 0;
}

// ─── Module reflection ────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_reflection_module_get_type(CHAOS_IL2CPP_INTPTR /*module*/, CHAOS_IL2CPP_INTPTR /*name*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_module_get_types(CHAOS_IL2CPP_INTPTR /*module*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_module_assembly(CHAOS_IL2CPP_INTPTR /*module*/) noexcept
{
    return 0;
}

// ─── Parameter reflection ─────────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_reflection_get_required_custom_modifiers(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INT32 chaos_reflection_get_param_attributes(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_default_value(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

// ─── Runtime helpers ──────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_formattablestring_factory_create(CHAOS_IL2CPP_INTPTR /*format*/, CHAOS_IL2CPP_INTPTR /*args*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_runtime_helpers_equals(CHAOS_IL2CPP_INTPTR /*left*/, CHAOS_IL2CPP_INTPTR /*right*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INT32 chaos_runtime_helpers_get_hash_code(CHAOS_IL2CPP_INTPTR /*value*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_runtime_helpers_get_object_value(CHAOS_IL2CPP_INTPTR /*value*/) noexcept
{
    return 0;
}

// ─── Method handle operations ─────────────────────────────────────

CHAOS_IL2CPP_INT32 chaos_runtimemethodhandle_get_hash_code(CHAOS_IL2CPP_INT64 /*handle*/) noexcept
{
    return 0;
}

// ─── Runtime-wrapped exception ────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_runtimewrapped_get_wrapped_exception(CHAOS_IL2CPP_INTPTR /*exc*/) noexcept
{
    return 0;
}

// ─── Reflection assignable ─────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_reflection_is_assignable_from(CHAOS_IL2CPP_INTPTR /*target*/, CHAOS_IL2CPP_INTPTR /*source*/) noexcept
{
    return 0;
}

// ─── Float32 marshalling ───────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_store_float32(CHAOS_IL2CPP_FLOAT32 value) noexcept
{
    CHAOS_IL2CPP_INT32 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return static_cast<CHAOS_IL2CPP_INTPTR>(bits);
}

// ─── Reflection generic ────────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_reflection_get_generic_param_constraints(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

// ─── Reflection member info ────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_virtual(CHAOS_IL2CPP_INTPTR /*member*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_base_definition(CHAOS_IL2CPP_INTPTR /*member*/) noexcept
{
    return 0;
}

// ─── Reflection field handle ───────────────────────────────────

CHAOS_IL2CPP_INT32 chaos_runtimefieldhandle_get_hash_code(CHAOS_IL2CPP_INTPTR /*handle*/) noexcept
{
    return 0;
}

// ─── Reflection module ─────────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_reflection_get_module_name(CHAOS_IL2CPP_INTPTR /*module*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_module_name_only(CHAOS_IL2CPP_INTPTR /*module*/) noexcept
{
    return 0;
}

// ─── Reflection parameter helpers ──────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_reflection_has_default_value(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_parameter_type(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_param_position(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

// ─── Reflection type handle ────────────────────────────────────

CHAOS_IL2CPP_INT32 chaos_runtimetypehandle_get_hash_code(CHAOS_IL2CPP_INTPTR /*handle*/) noexcept
{
    return 0;
}

// ─── Reflection constructors ───────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_reflection_get_constructors_default(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}


// ─── Reflection assignable (supplementary) ──────────────────────

CHAOS_IL2CPP_INTPTR chaos_reflection_is_assignable_to(CHAOS_IL2CPP_INTPTR /*target*/, CHAOS_IL2CPP_INTPTR /*source*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_is_instance_of_type(CHAOS_IL2CPP_INTPTR /*obj*/, CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

// ─── Assembly reflection ────────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_reflection_get_image_runtime_version(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_constructed_generic(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_interface(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_array(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_assembly_full_name(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_generic_param_pos(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_enum(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_abstract(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_assembly_qualified_name(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_assembly_get_types(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_calling_assembly(void) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_entry_assembly(void) noexcept
{
    return 0;
}

// Reflection supplementary
CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_sealed(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_is_generic_type_def(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_from_assembly_bool(CHAOS_IL2CPP_INTPTR /*assembly*/, CHAOS_IL2CPP_INTPTR /*name*/, CHAOS_IL2CPP_INT32 /*throw_on_error*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_executing_assembly(void) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_assembly_location(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept
{
    return 0;
}

// ─── Generic type reflection ────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_reflection_make_generic_type(CHAOS_IL2CPP_INTPTR /*def*/, CHAOS_IL2CPP_INTPTR /*args*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_contains_generic_params(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

// ─── Parameter default value ────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_reflection_get_raw_default_value(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

// ─── Type reflection ────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR chaos_reflection_get_fields_bindingflags(CHAOS_IL2CPP_INTPTR /*type*/, CHAOS_IL2CPP_INT32 /*flags*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_methods_bindingflags(CHAOS_IL2CPP_INTPTR /*type*/, CHAOS_IL2CPP_INT32 /*flags*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_base_type(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_get_type_full_name(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}


// ─── Volatile operations ──────────────────────────────────────────

CHAOS_IL2CPP_INT32 chaos_volatile_read(CHAOS_IL2CPP_INTPTR ptr) noexcept
{
    return *reinterpret_cast<volatile CHAOS_IL2CPP_INT32*>(ptr);
}

// -- Generic registration callback (defined by generated code via static init) --
extern "C" void (*g_chaos_populate_generic_registration)(void) = nullptr;

}  // extern "C"
