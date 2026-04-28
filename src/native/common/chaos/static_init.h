#ifndef CHAOS_IL2CPP_COMMON_STATIC_INIT_H_
#define CHAOS_IL2CPP_COMMON_STATIC_INIT_H_

#include <mutex>

namespace ChaosIl2cpp::Common {

template <typename TInitFunc>
inline void ensure_initialized(CHAOS_IL2CPP_ONCE_FLAG& flag, TInitFunc init)
{
    CHAOS_IL2CPP_CALL_ONCE(flag, static_cast<TInitFunc&&>(init));
}

} // namespace ChaosIl2cpp::Common

#endif // CHAOS_IL2CPP_COMMON_STATIC_INIT_H_
