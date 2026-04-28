#ifndef CHAOS_IL2CPP_COMMON_PTR_TAG_H_
#define CHAOS_IL2CPP_COMMON_PTR_TAG_H_

#include <cstdint>

namespace ChaosIl2cpp::Common {

constexpr std::intptr_t k_managed_pointer_local_slot_tag = 1;
constexpr std::intptr_t k_raw_int32_pointer_tag = 2;

inline std::intptr_t* resolve_native_int_slot(std::intptr_t address) noexcept
{
    if ((address & k_managed_pointer_local_slot_tag) != 0)
    {
        return reinterpret_cast<std::intptr_t*>(
            static_cast<std::uintptr_t>(address & ~k_managed_pointer_local_slot_tag));
    }
    return reinterpret_cast<std::intptr_t*>(address);
}

inline void* resolve_block_copy_address(std::intptr_t address) noexcept
{
    if ((address & k_raw_int32_pointer_tag) != 0)
    {
        return reinterpret_cast<void*>(
            static_cast<std::uintptr_t>(address & ~k_raw_int32_pointer_tag));
    }
    if ((address & k_managed_pointer_local_slot_tag) != 0)
    {
        return reinterpret_cast<void*>(
            static_cast<std::uintptr_t>(address & ~k_managed_pointer_local_slot_tag));
    }
    return reinterpret_cast<void*>(address);
}

} // namespace ChaosIl2cpp::Common

#endif // CHAOS_IL2CPP_COMMON_PTR_TAG_H_
