#ifndef CHAOS_IL2CPP_DELEGATE_OBJECT_H_
#define CHAOS_IL2CPP_DELEGATE_OBJECT_H_

#include <chaos/native_types.h>

#include <cstdint>

namespace chaos::il2cpp::runtime_core {

// ── DelegateObject ──────────────────────────────────────────────────────
// 56-byte unified struct matching the scriban template's codegen output
// (chaos_type_System_Private_CoreLib_System_Delegate).
//
// Layout:
//   [0..7]   void* type_info
//   [8..15]  uint64_t sync_state (ThinLockableHeader)
//   [16..23] intptr_t chaos_delegate_target
//   [24..31] intptr_t chaos_delegate_method_ptr
//   [32..39] intptr_t chaos_delegate_invocation_list (vector<INTPTR>*)
//   [40..47] intptr_t chaos_delegate_invocation_count
//   [48..51] uint32_t chaos_delegate_method_token
//   [52..55] uint32_t _pad
struct DelegateObject {
    void* type_info;
    uint64_t sync_state;
    CHAOS_IL2CPP_INTPTR chaos_delegate_target;
    CHAOS_IL2CPP_INTPTR chaos_delegate_method_ptr;
    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_list;
    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_count;
    CHAOS_IL2CPP_UINT32 chaos_delegate_method_token;
    CHAOS_IL2CPP_UINT32 _pad;
};
static_assert(sizeof(DelegateObject) == 56, "DelegateObject must be 56 bytes");

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_DELEGATE_OBJECT_H_
