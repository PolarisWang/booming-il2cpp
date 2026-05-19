// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// exception_api.cpp — Native AOT exception support implementation
//
// Provides extern "C" implementations for exception metadata functions
// called from generated C++ code via SimpleForward dispatch.
//
// Exception objects in AOT mode use the generated chaos_type_* struct layout
// (defined in each family's native-aot.generated.cpp). The common prefix is:
//
//   struct chaos_type_System_Private_CoreLib_System_Exception {
//       ThinLockableHeader header;          // 16 bytes (type_info + sync_state)
//       CHAOS_IL2CPP_INTPTR _message;        // offset +16
//       CHAOS_IL2CPP_INTPTR _innerException; // offset +24
//       CHAOS_IL2CPP_INTPTR _stackTrace;     // offset +32
//       CHAOS_IL2CPP_INT32 _HResult;         // offset +40
//   };
//
// The header size is sizeof(ThinLockableHeader) = 16 bytes (64-bit)

#include "runtime_core.h"
#include "module_registry.h"
#include <chaos/trace.h>
#include <chaos/type_info.h>

#include <cstdio>
#include <cstring>

namespace chaos::il2cpp::runtime_core {

// Offset of _message field in Exception struct (after header)
// Generated types use ThinLockableHeader (16B: type_info + sync_state).
static constexpr CHAOS_IL2CPP_SIZE kExceptionMessageOffset = sizeof(ThinLockableHeader);
static constexpr CHAOS_IL2CPP_SIZE kExceptionInnerExceptionOffset = sizeof(ThinLockableHeader) + sizeof(CHAOS_IL2CPP_INTPTR);
static constexpr CHAOS_IL2CPP_SIZE kExceptionStackTraceOffset = sizeof(ThinLockableHeader) + 2 * sizeof(CHAOS_IL2CPP_INTPTR);
static constexpr CHAOS_IL2CPP_SIZE kExceptionHResultOffset = sizeof(ThinLockableHeader) + 3 * sizeof(CHAOS_IL2CPP_INTPTR);

static inline CHAOS_IL2CPP_INTPTR* GetExceptionFieldPtr(void* exception_obj, CHAOS_IL2CPP_SIZE field_offset) {
    if (exception_obj == nullptr) return nullptr;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(
        static_cast<CHAOS_IL2CPP_UINT8*>(exception_obj) + field_offset);
}

extern "C" void ChaosReflectionSetExceptionMetadata(
    CHAOS_IL2CPP_INTPTR exception_obj,
    CHAOS_IL2CPP_INTPTR message_value)
{
    CHAOS_IL2CPP_LOG_TRACE("runtime", "SetExceptionMetadata", "");
    auto* message_slot = GetExceptionFieldPtr(
        reinterpret_cast<void*>(exception_obj),
        kExceptionMessageOffset);
    if (message_slot == nullptr) return;

    *message_slot = message_value;
}

extern "C" void ChaosReflectionSetExceptionMetadata_2params(
    CHAOS_IL2CPP_INTPTR exception_obj,
    CHAOS_IL2CPP_INTPTR message_value,
    CHAOS_IL2CPP_INTPTR param_name_value)
{
    auto* message_slot = GetExceptionFieldPtr(
        reinterpret_cast<void*>(exception_obj),
        kExceptionMessageOffset);
    if (message_slot != nullptr) {
        *message_slot = message_value;
    }

    (void)param_name_value;
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetExceptionMessage(
    CHAOS_IL2CPP_INTPTR exception_obj)
{
    auto* message_slot = GetExceptionFieldPtr(
        reinterpret_cast<void*>(exception_obj),
        kExceptionMessageOffset);
    if (message_slot == nullptr) return 0;

    return *message_slot;
}

extern "C" CHAOS_IL2CPP_INTPTR chaos_reflection_get_exception_param_name(
    CHAOS_IL2CPP_INTPTR exception_obj)
{
    (void)exception_obj;
    return 0;
}

extern "C" void ChaosReflectionInitDefaultException(
    CHAOS_IL2CPP_INTPTR exception_obj)
{
    auto* runtime = GetCurrentRuntimeState();
    auto* thread = GetCurrentThreadState();
    auto* abi = GetRuntimeAbiV0();
    if (runtime == nullptr || thread == nullptr || abi == nullptr) return;

    auto* obj = reinterpret_cast<void*>(exception_obj);
    if (obj == nullptr) return;
    auto* type_info = chaos_object_get_type_info(obj);
    if (type_info == nullptr) return;

    // Look up the exception's class name from the module registry.
    const char* ns = nullptr;
    const char* type_name = LookupTypeNameByInfoPtr(type_info, &ns);
    if (type_name == nullptr) type_name = "Exception";

    // Format: "Exception of type 'Namespace.TypeName' was thrown."
    char buffer[256];
    const char* fmt_str = "Exception of type '%s' was thrown.";
    if (ns != nullptr && ns[0] != '\0') {
        // Use a temporary format with the namespace prefix
        char qualified_name[128];
        std::snprintf(qualified_name, sizeof(qualified_name), "%s.%s", ns, type_name);
        std::snprintf(buffer, sizeof(buffer), fmt_str, qualified_name);
    } else {
        std::snprintf(buffer, sizeof(buffer), fmt_str, type_name);
    }

    auto* msg_obj = abi->string_new_utf8(runtime, thread, buffer,
        static_cast<uintptr_t>(std::strlen(buffer)));
    if (msg_obj != nullptr) {
        auto* message_slot = GetExceptionFieldPtr(obj, kExceptionMessageOffset);
        if (message_slot != nullptr) {
            *message_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(msg_obj);
        }
    }
}

}  // namespace chaos::il2cpp::runtime_core
