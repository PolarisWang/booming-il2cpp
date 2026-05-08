// exception_api.cpp — Native AOT exception support implementation
//
// Provides extern "C" implementations for exception metadata functions
// called from generated C++ code via SimpleForward dispatch.
//
// Exception objects in AOT mode use the generated chaos_type_* struct layout
// (defined in each family's native-aot.generated.cpp). The common prefix is:
//
//   struct chaos_type_System_Private_CoreLib_System_Exception {
//       FatHeader header;                // 24 bytes (type_info + vtable + sync_state)
//       CHAOS_IL2CPP_INTPTR _message;        // offset +24
//       CHAOS_IL2CPP_INTPTR _innerException; // offset +32
//       CHAOS_IL2CPP_INTPTR _stackTrace;     // offset +40
//       CHAOS_IL2CPP_INT32 _HResult;         // offset +48
//   };
//
// The header size is sizeof(FatHeader) = 24 bytes (64-bit)

#include "runtime_core.h"
#include <chaos/trace.h>
#include <chaos/type_info.h>

#include <cstring>

namespace chaos::il2cpp::runtime_core {

// Offset of _message field in Exception struct (after header)
// Generated types use FatHeader (24B: type_info + vtable + sync_state).
static constexpr CHAOS_IL2CPP_SIZE kExceptionMessageOffset = sizeof(FatHeader);
static constexpr CHAOS_IL2CPP_SIZE kExceptionInnerExceptionOffset = sizeof(FatHeader) + sizeof(CHAOS_IL2CPP_INTPTR);
static constexpr CHAOS_IL2CPP_SIZE kExceptionStackTraceOffset = sizeof(FatHeader) + 2 * sizeof(CHAOS_IL2CPP_INTPTR);
static constexpr CHAOS_IL2CPP_SIZE kExceptionHResultOffset = sizeof(FatHeader) + 3 * sizeof(CHAOS_IL2CPP_INTPTR);

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

}  // namespace chaos::il2cpp::runtime_core
