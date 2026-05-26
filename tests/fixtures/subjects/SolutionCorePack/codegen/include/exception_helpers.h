#ifndef CHAOS_IL2CPP_EXCEPTION_HELPERS_H_
#define CHAOS_IL2CPP_EXCEPTION_HELPERS_H_

#include "module_registry.h"

#include <cstddef>
#include <cstdint>

namespace chaos::il2cpp::runtime_core {

/// Resolve a fully-qualified managed type name (e.g. "System.InvalidCastException")
/// to a TypeInfoHandle by scanning all registered modules.
/// Returns 0 if the type cannot be found in any module.
TypeInfoHandle ResolveTypeByName(const char* fully_qualified_name);

/// Create and raise a managed exception of the given type with the given message.
/// This function never returns — it either raises the managed exception or aborts
/// if the type cannot be resolved or the runtime is not yet initialized.
[[noreturn]] void RaiseManagedException(const char* type_full_name, const char* message_utf8);

/// Register a fallback callback for Fact Static verification mode.
/// When RaiseManagedException detects an uninitialized runtime, it calls this
/// callback instead of aborting. The callback must NOT return (typically uses
/// longjmp back to the verification harness).
extern "C" void SetExceptionFallback(void (*fn)());

/// Register a fallback callback for Fact Static verification mode.
/// When RaiseManagedException detects an uninitialized runtime, it calls this
/// callback instead of aborting. The callback must NOT return (typically uses
/// longjmp back to the verification harness).
extern "C" void SetExceptionFallback(void (*fn)());

// ── Convenience inline wrappers for common BCL exception types ──

[[noreturn]] inline void RaiseInvalidCastException() {
    RaiseManagedException("System.InvalidCastException",
        "Specified cast is not valid.");
}

[[noreturn]] inline void RaiseFormatException() {
    RaiseManagedException("System.FormatException",
        "Input string was not in a correct format.");
}

[[noreturn]] inline void RaiseArgumentNullException(const char* param_name) {
    (void)param_name;
    RaiseManagedException("System.ArgumentNullException",
        "Value cannot be null.");
}

/// Throws a managed ArgumentException.
[[noreturn]] inline void RaiseArgumentException(const char* message) {
    RaiseManagedException("System.ArgumentException", message);
}

/// Throws a managed NullReferenceException.
/// Uses RaiseManagedException to resolve the type by name and dispatch through
/// the runtime ABI.  This is the correct exception for callvirt on null `this`,
/// replacing the old codegen pattern that called CHAOS_IL2CPP_FAIL().
[[noreturn]] inline void RaiseNullReferenceException() {
    RaiseManagedException("System.NullReferenceException",
        "Object reference not set to an instance of an object.");
}

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_EXCEPTION_HELPERS_H_
