#ifndef CHAOS_IL2CPP_DELEGATE_OBJECT_H_
#define CHAOS_IL2CPP_DELEGATE_OBJECT_H_

#include <chaos/native_types.h>

#include <cstdint>

namespace chaos::il2cpp::runtime_core {

// ── DelegateObject ──────────────────────────────────────────────────────
// 48-byte unified struct matching the ThinLockableHeader-optimized codegen.
// sync_state removed — moved to ThinLockTable (8B saved per object).
//
// Layout:
//   [0..7]   void* type_info
//   [8..15]  intptr_t chaos_delegate_target
//   [16..23] intptr_t chaos_delegate_method_ptr
//   [24..31] intptr_t chaos_delegate_invocation_list (vector<INTPTR>*)
//   [32..39] intptr_t chaos_delegate_invocation_count
//   [40..43] uint32_t chaos_delegate_method_token
//   [44..47] uint32_t _pad
struct DelegateObject {
    void* type_info;
    CHAOS_IL2CPP_INTPTR chaos_delegate_target;
    CHAOS_IL2CPP_INTPTR chaos_delegate_method_ptr;
    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_list;
    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_count;
    CHAOS_IL2CPP_UINT32 chaos_delegate_method_token;
    CHAOS_IL2CPP_UINT32 _pad;
};
static_assert(sizeof(DelegateObject) == 48, "DelegateObject must be 48 bytes");

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_DELEGATE_OBJECT_H_
