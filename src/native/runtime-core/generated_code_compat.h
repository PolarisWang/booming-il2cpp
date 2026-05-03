#ifndef CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_
#define CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_

// ── Compatibility declarations for older generated .cpp files ──
//
// This header provides fallback declarations for symbols that older versions
// of native-aot.generated.cpp reference by bare name (without namespace
// qualification or extern "C" declarations). These files were generated before
// header restructuring (non-inline extern "C" wrappers, type IDs, struct types
// moved to shared headers) and need compat declarations to compile under MSVC.
//
// Each block documents which family and symbol it covers.
//
// When ALL families have been regenerated through the batch pipeline, this
// entire file can be deleted.

#include <chaos/native_types.h>

// ── Missing type IDs ──
// Older generated .cpp files declare only a subset of type IDs.
// These fill gaps in type_id constexpr constants.
// NOTE: chaos_type_id_System_Private_CoreLib_System_Object = 2 is NOT
// defined here because some generated files already define it in their
// anonymous namespace, causing C2872 ambiguous symbol errors.

constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = 3;
constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = 7;

// ── Missing valuetype structs ──
// Some generated files (enum-parsing) reference chaos_valuetype_* structs
// in boxed type definitions but don't declare the valuetype struct itself.
// These are simple wrappers around the underlying type.
struct chaos_valuetype_System_Private_CoreLib_System_DayOfWeek {
    CHAOS_IL2CPP_INT32 value = 0;
};

// ── Object header used by generated reflection struct types ──
struct chaos_object_header {
    CHAOS_IL2CPP_INTPTR type_id = 0;
};

// ── Reflection struct type declarations ──
// Used by: boxing-unboxing-casts, attributes-custom-metadata, enum-parsing
// (these families have inline code that creates/reinterprets chaos_type_*
//  structs that may not be declared in their own generated .cpp)

struct chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo {
    chaos_object_header header{};
    CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;
    CHAOS_IL2CPP_INTPTR runtime_method_handle = 0;
    CHAOS_IL2CPP_INTPTR generic_definition_method_handle = 0;
    CHAOS_IL2CPP_INTPTR generic_argument_type_handle = 0;
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

// ── Collection/monitor runtime storage helpers (template wrappers) ──
// Used by: collections-generic-core, threading-monitor-interlocked
// Older generated code calls these as bare template function names.

#include "collection.h"
#include "monitor.h"

template <typename TKey, typename TValue>
inline auto* chaos_require_dictionary_runtime_storage(CHAOS_IL2CPP_INTPTR handle) {
    return ChaosIl2cpp::Common::require_dictionary_runtime_storage<TKey, TValue>(handle);
}

inline auto& chaos_require_monitor_runtime_entry(CHAOS_IL2CPP_INTPTR object_value) {
    return ChaosIl2cpp::Common::require_monitor_runtime_entry(object_value);
}

// ── Exception metadata functions (extern C declarations) ──
// Used by: exception-throw-diagnostics
// Implementations live in exception_api.cpp.

extern "C" {
void chaos_reflection_set_exception_metadata(CHAOS_IL2CPP_INTPTR exception_obj, CHAOS_IL2CPP_INTPTR message_value, CHAOS_IL2CPP_INTPTR param_name_value);
CHAOS_IL2CPP_INTPTR chaos_reflection_get_exception_message(CHAOS_IL2CPP_INTPTR exception_obj);
}  // extern "C"

// ── Typedef for collection callback used by collections-generic-core ──
typedef CHAOS_IL2CPP_INTPTR (*chaos_callback_func)(CHAOS_IL2CPP_INTPTR);

// ── Decimal bridge for older generated code ──
// chaos_decimal_ctor_int32 is declared in namespace chaos::il2cpp::runtime_core.
// Older generated code calls it as a bare identifier.
// NOTE: The `using` declaration for this is in runtime_core.h (Phase 1 namespace block)
// so it's visible inside anonymous namespaces in generated .cpp files.

#endif  // CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_
