#ifndef CHAOS_IL2CPP_COMMON_FINALLY_H_
#define CHAOS_IL2CPP_COMMON_FINALLY_H_

#include <type_traits>

namespace ChaosIl2cpp::Common {

template <typename TAction>
class FinallyGuard
{
public:
    explicit FinallyGuard(TAction action) noexcept(std::is_nothrow_move_constructible_v<TAction>)
        : action_(static_cast<TAction&&>(action))
    {
    }

    FinallyGuard(const FinallyGuard&) = delete;
    FinallyGuard& operator=(const FinallyGuard&) = delete;

    ~FinallyGuard() noexcept(noexcept(std::declval<TAction&>()))
    {
        if (active_)
        {
            action_();
        }
    }

    void dismiss() noexcept { active_ = false; }

private:
    TAction action_;
    bool active_ = true;
};

template <typename TAction>
FinallyGuard<TAction> make_finally_guard(TAction action)
{
    return FinallyGuard<TAction>(action);
}

} // namespace ChaosIl2cpp::Common

#endif // CHAOS_IL2CPP_COMMON_FINALLY_H_
