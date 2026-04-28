#ifndef CHAOS_IL2CPP_COMMON_STATIC_INIT_H_
#define CHAOS_IL2CPP_COMMON_STATIC_INIT_H_

#include <mutex>

namespace ChaosIl2cpp::Common {

template <typename TInitFunc>
inline void ensure_initialized(std::once_flag& flag, TInitFunc init)
{
    std::call_once(flag, static_cast<TInitFunc&&>(init));
}

} // namespace ChaosIl2cpp::Common

#endif // CHAOS_IL2CPP_COMMON_STATIC_INIT_H_
