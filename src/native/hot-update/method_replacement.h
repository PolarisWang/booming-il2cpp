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

bool Register(CHAOS_IL2CPP_UINT32 method_token, void* thunk);
bool Revert(CHAOS_IL2CPP_UINT32 method_token);
void RevertAll();
void* Resolve(CHAOS_IL2CPP_UINT32 method_token);
CHAOS_IL2CPP_UINT32 ActiveCount();

}  // namespace chaos::il2cpp::method_replacement

#endif  // CHAOS_IL2CPP_METHOD_REPLACEMENT_H_
