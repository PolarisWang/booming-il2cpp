#ifndef CHAOS_IL2CPP_COMMON_PTR_TAG_H_
#define CHAOS_IL2CPP_COMMON_PTR_TAG_H_

#include <cstdint>

namespace chaos::il2cpp::common {

constexpr CHAOS_IL2CPP_INTPTR k_managed_pointer_local_slot_tag = 1;
constexpr CHAOS_IL2CPP_INTPTR k_raw_int32_pointer_tag = 2;

inline CHAOS_IL2CPP_INTPTR* resolve_native_int_slot(CHAOS_IL2CPP_INTPTR address) noexcept
{
    if ((address & k_managed_pointer_local_slot_tag) != 0)
    {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(
            static_cast<CHAOS_IL2CPP_UINTPTR>(address & ~k_managed_pointer_local_slot_tag));
    }
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(address);
}

inline void* resolve_block_copy_address(CHAOS_IL2CPP_INTPTR address) noexcept
{
    if ((address & k_raw_int32_pointer_tag) != 0)
    {
        return reinterpret_cast<void*>(
            static_cast<CHAOS_IL2CPP_UINTPTR>(address & ~k_raw_int32_pointer_tag));
    }
    if ((address & k_managed_pointer_local_slot_tag) != 0)
    {
        return reinterpret_cast<void*>(
            static_cast<CHAOS_IL2CPP_UINTPTR>(address & ~k_managed_pointer_local_slot_tag));
    }
    return reinterpret_cast<void*>(address);
}

} // namespace chaos::il2cpp::common

#endif // CHAOS_IL2CPP_COMMON_PTR_TAG_H_
