#ifndef CHAOS_IL2CPP_METHOD_REPLACEMENT_H_
#define CHAOS_IL2CPP_METHOD_REPLACEMENT_H_

#include <cstdint>

namespace chaos::il2cpp::method_replacement {

struct MethodReplacementEntry {
    uint32_t method_token = 0u;
    void* original_pointer = nullptr;
    void* replacement_thunk = nullptr;
    bool active = false;
};

bool Register(uint32_t method_token, void* thunk);
bool Revert(uint32_t method_token);
void RevertAll();
void* Resolve(uint32_t method_token);
uint32_t ActiveCount();

}  // namespace chaos::il2cpp::method_replacement

#endif  // CHAOS_IL2CPP_METHOD_REPLACEMENT_H_
