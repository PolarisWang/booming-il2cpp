#ifndef CHAOS_IL2CPP_METHOD_REPLACEMENT_H_
#define CHAOS_IL2CPP_METHOD_REPLACEMENT_H_

#include <chaos/native_types.h>

#include <cstdint>

namespace chaos::il2cpp::method_replacement {

struct MethodReplacementEntry {
    CHAOS_IL2CPP_UINT32 method_token = 0u;
    void* original_pointer = nullptr;
    void* replacement_thunk = nullptr;
    bool active = false;
};

/// Register a method replacement for the given method_token.
///
/// @return true  if the replacement was accepted as a binding (token and thunk
///               are valid).  This does NOT guarantee that the method is a real
///               dispatch subject — a token accepted by Register may still be an
///               inert entry if no codegen dispatch slot exists for it.
///               Effectiveness is determined by the loader/API boundary:
///                 - ChaosApplyPatch returns method_count + NO_METHODS/PARTIAL
///                 - SetPatchedBySlot reports actual dispatch slot wiring
///                 - Direct callers of Register (outside ChaosApplyPatch) must
///                   separately verify that Resolve() returns the replacement
///                   thunk before treating the patch as live.
/// @return false if method_token==0 or thunk==nullptr.
bool Register(CHAOS_IL2CPP_UINT32 method_token, void* thunk);
bool Revert(CHAOS_IL2CPP_UINT32 method_token);
void RevertAll();
void* Resolve(CHAOS_IL2CPP_UINT32 method_token);
CHAOS_IL2CPP_UINT32 ActiveCount();

}  // namespace chaos::il2cpp::method_replacement

#endif  // CHAOS_IL2CPP_METHOD_REPLACEMENT_H_
