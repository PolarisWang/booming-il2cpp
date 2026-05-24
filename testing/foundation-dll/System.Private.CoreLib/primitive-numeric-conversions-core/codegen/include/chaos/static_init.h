#ifndef CHAOS_IL2CPP_COMMON_STATIC_INIT_H_
#define CHAOS_IL2CPP_COMMON_STATIC_INIT_H_

#include <mutex>

namespace chaos::il2cpp::common {

template <typename TInitFunc>
inline void ensure_initialized(CHAOS_IL2CPP_ONCE_FLAG& flag, TInitFunc init)
{
    CHAOS_IL2CPP_CALL_ONCE(flag, static_cast<TInitFunc&&>(init));
}

} // namespace chaos::il2cpp::common

#endif // CHAOS_IL2CPP_COMMON_STATIC_INIT_H_
