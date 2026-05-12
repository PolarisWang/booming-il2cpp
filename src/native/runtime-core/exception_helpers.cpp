// exception_helpers.cpp — Runtime exception raising for generated code
//
// Provides ResolveTypeByName (module-scanned type resolution) and
// RaiseManagedException (managed exception creation + raising) used by
// external runtime helpers in generated C++ code.
//
// Design:
//   External helpers have TLS access (GetCurrentRuntimeState/ThreadState)
//   via runtime_core.h. On exception types not yet loaded, ResolveTypeByName
//   iterates all registered modules and finds the type by (namespace, name).
//   The TypeInfoHandle is used with abi->object_new() to create the exception,
//   then abi->raise_managed_exception() to throw it.
//
//   Pattern matches the existing scriban template: creates message string
//   but uses (void)msg_obj for the constructor call (matches current behavior).

#include "exception_helpers.h"

#include "runtime_core.h"

#include <cstdlib>
#include <cstring>

// ── Fact Static verification fallback: callback instead of abort ──────────
//
// When the runtime is not fully initialized (Fact Static verification mode), the
// generated code's throwing helpers call into RaiseManagedException, which
// would hit std::abort() -> fast-fail (0xC0000409).  That is uncatchable.
//
// Instead, the Fact Static verification harness registers a fallback callback via
// SetExceptionFallback().  When RaiseManagedException detects an
// uninitialized runtime, it calls the fallback instead of aborting.
//
// The fallback is expected to NOT return (e.g., use longjmp).  If no
// fallback is registered, std::abort() is still called.
//
// This avoids C++ exception propagation through extern "C" frames, which
// requires non-default MSVC EH flags (/EHa or /EHs).

static thread_local void (*g_exception_fallback)() = nullptr;

extern "C" void SetExceptionFallback(void (*fn)()) {
    g_exception_fallback = fn;
}

namespace chaos::il2cpp::runtime_core {

// ── Helpers ──────────────────────────────────────────────────────────────

// Forward-declare the extern "C" function from exception_api.cpp.
// Both files are compiled into the same chaos_runtime_core library.
extern "C" void ChaosReflectionSetExceptionMetadata(
    CHAOS_IL2CPP_INTPTR exception_obj,
    CHAOS_IL2CPP_INTPTR message_value);

// ── ResolveTypeByName ────────────────────────────────────────────────────

TypeInfoHandle ResolveTypeByName(const char* fully_qualified_name) {
    if (fully_qualified_name == nullptr || fully_qualified_name[0] == '\0') {
        return 0;
    }

    // Split at the LAST '.' to get namespace and type name.
    const char* last_dot = nullptr;
    for (const char* p = fully_qualified_name; *p != '\0'; ++p) {
        if (*p == '.') {
            last_dot = p;
        }
    }

    const char* type_name = nullptr;
    const char* namespace_start = fully_qualified_name;
    size_t namespace_len = 0;

    if (last_dot != nullptr) {
        namespace_len = static_cast<size_t>(last_dot - fully_qualified_name);
        type_name = last_dot + 1;
    } else {
        // No dot — treat everything as type name with empty namespace.
        type_name = fully_qualified_name;
    }

    // Scan all registered modules. BCL namespaces are short (< 256), so use
    // a stack buffer for the null-terminated namespace copy.
    char namespace_buf[256];
    const char* ns_for_lookup = nullptr;

    if (last_dot != nullptr) {
        if (namespace_len >= sizeof(namespace_buf)) {
            return 0;
        }
        std::memcpy(namespace_buf, fully_qualified_name, namespace_len);
        namespace_buf[namespace_len] = '\0';
        ns_for_lookup = namespace_buf;
    } else {
        ns_for_lookup = "";
    }

    for (uint32_t i = 0; i < GetModuleCount(); ++i) {
        const auto* mod = GetModuleByIndex(i);
        if (mod == nullptr || mod->image == nullptr) {
            continue;
        }

        const auto* type = FindReflectionQueryTypeByName(
            mod->image, ns_for_lookup, type_name);
        if (type != nullptr) {
            return MakeTypeHandle(i, type->metadata_token);
        }
    }

    return 0;
}

// ── RaiseManagedException ────────────────────────────────────────────────

[[noreturn]] void RaiseManagedException(
    const char* type_full_name, const char* message_utf8)
{
    auto* runtime = GetCurrentRuntimeState();
    auto* thread = GetCurrentThreadState();
    auto* abi = GetRuntimeAbiV0();

    if (runtime == nullptr || thread == nullptr || abi == nullptr) {
        if (g_exception_fallback != nullptr) {
            g_exception_fallback();
        }
        std::abort();
    }

    const auto type_handle = ResolveTypeByName(type_full_name);
    if (type_handle == 0) {
        // Per-family build: exception type not registered.
        // Throw empty chaos_managed_exception — the codegen catch block
        // treats object_value == 0 as "matched but no managed object",
        // allowing the catch body to execute in verification/benchmark mode.
        throw chaos_managed_exception{0};
    }

    abi->class_init(runtime, type_handle);

    auto* exc = abi->object_new(runtime, thread, type_handle);
    if (exc == nullptr) {
        std::abort();
    }

    if (message_utf8 != nullptr && message_utf8[0] != '\0') {
        const auto msg_len = std::strlen(message_utf8);
        auto* msg_obj = abi->string_new_utf8(runtime, thread, message_utf8, msg_len);
        if (msg_obj != nullptr) {
            ChaosReflectionSetExceptionMetadata(
                reinterpret_cast<CHAOS_IL2CPP_INTPTR>(exc),
                reinterpret_cast<CHAOS_IL2CPP_INTPTR>(msg_obj));
        }
    }

    abi->raise_managed_exception(runtime, thread,
        reinterpret_cast<ExceptionHandle>(exc));

    std::abort();
}

}  // namespace chaos::il2cpp::runtime_core
