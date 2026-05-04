// exception_api.cpp — Native AOT exception support implementation
//
// Provides extern "C" implementations for exception metadata functions
// called from generated C++ code via SimpleForward dispatch.
//
// Exception objects in AOT mode use the generated chaos_type_* struct layout
// (defined in each family's native-aot.generated.cpp). The common prefix is:
//
//   struct chaos_type_System_Private_CoreLib_System_Exception {
//       chaos_object_header header;          // 8 bytes (type_info*)
//       CHAOS_IL2CPP_INTPTR _message;        // offset +8
//       CHAOS_IL2CPP_INTPTR _innerException; // offset +16
//       CHAOS_IL2CPP_INTPTR _stackTrace;     // offset +24
//       CHAOS_IL2CPP_INT32 _HResult;         // offset +32
//   };
//
// The header size is sizeof(chaos_object_header) = sizeof(CHAOS_IL2CPP_INTPTR) = 8 bytes (64-bit)
// Message field is at offset 8 from the object base.

#include "runtime_core.h"
#include <chaos/type_info.h>

#include <cstring>

// chaos_object_header is defined per-family in generated code.
// This local definition (same layout) is needed for sizeof calculations
// in exception field offset computation.
struct chaos_object_header
{
    const TypeInfo* type_info = nullptr;
};

namespace chaos::il2cpp::runtime_core {

// Offset of _message field in Exception struct (after header)
static constexpr CHAOS_IL2CPP_SIZE kExceptionMessageOffset = sizeof(chaos_object_header);
static constexpr CHAOS_IL2CPP_SIZE kExceptionInnerExceptionOffset = sizeof(chaos_object_header) + sizeof(CHAOS_IL2CPP_INTPTR);
static constexpr CHAOS_IL2CPP_SIZE kExceptionStackTraceOffset = sizeof(chaos_object_header) + 2 * sizeof(CHAOS_IL2CPP_INTPTR);
static constexpr CHAOS_IL2CPP_SIZE kExceptionHResultOffset = sizeof(chaos_object_header) + 3 * sizeof(CHAOS_IL2CPP_INTPTR);

static inline CHAOS_IL2CPP_INTPTR* GetExceptionFieldPtr(void* exception_obj, CHAOS_IL2CPP_SIZE field_offset) {
    if (exception_obj == nullptr) return nullptr;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(
        static_cast<CHAOS_IL2CPP_UINT8*>(exception_obj) + field_offset);
}

}  // namespace chaos::il2cpp::runtime_core

extern "C" void ChaosReflectionSetExceptionMetadata(
    CHAOS_IL2CPP_INTPTR exception_obj,
    CHAOS_IL2CPP_INTPTR message_value)
{
    using namespace chaos::il2cpp::runtime_core;

    auto* message_slot = GetExceptionFieldPtr(
        reinterpret_cast<void*>(exception_obj),
        kExceptionMessageOffset);
    if (message_slot == nullptr) return;

    *message_slot = message_value;
}

extern "C" void ChaosReflectionSetExceptionMetadata(
    CHAOS_IL2CPP_INTPTR exception_obj,
    CHAOS_IL2CPP_INTPTR message_value,
    CHAOS_IL2CPP_INTPTR param_name_value)
{
    using namespace chaos::il2cpp::runtime_core;

    // Set message field
    auto* message_slot = GetExceptionFieldPtr(
        reinterpret_cast<void*>(exception_obj),
        kExceptionMessageOffset);
    if (message_slot != nullptr) {
        *message_slot = message_value;
    }

    // For ArgumentException subclasses, param_name is stored in a subclass field
    // at a different offset. For the generic case, we don't set it here.
    (void)param_name_value;
}

extern "C" void ChaosReflectionSetExceptionMetadata_2params(
    CHAOS_IL2CPP_INTPTR exception_obj,
    CHAOS_IL2CPP_INTPTR message_value,
    CHAOS_IL2CPP_INTPTR param_name_value)
{
    using namespace chaos::il2cpp::runtime_core;

    // Set message field
    auto* message_slot = GetExceptionFieldPtr(
        reinterpret_cast<void*>(exception_obj),
        kExceptionMessageOffset);
    if (message_slot != nullptr) {
        *message_slot = message_value;
    }

    // For ArgumentException subclasses, param_name is stored in a subclass field
    // at a different offset. For the generic case, we don't set it here.
    (void)param_name_value;
}

extern "C" CHAOS_IL2CPP_INTPTR ChaosReflectionGetExceptionMessage(
    CHAOS_IL2CPP_INTPTR exception_obj)
{
    using namespace chaos::il2cpp::runtime_core;

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
    // ParamName is specific to ArgumentException subclasses.
    // Full implementation requires knowing the exact subclass layout.
    return 0;
}
