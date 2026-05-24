// jit_registry_lock_guard.h — RAII spinlock guard for JIT SEH handler registry
//
// Provides exception-safe lock acquisition/release for the T4 code registry
// spinlock used in WinSehHandler and LinuxSehHandler.  Replaces raw
// AcquireLock()/ReleaseLock() pairs with a scoped guard that auto-releases
// on scope exit (including early returns and exceptions).

#ifndef CHAOS_IL2CPP_JIT_REGISTRY_LOCK_GUARD_H_
#define CHAOS_IL2CPP_JIT_REGISTRY_LOCK_GUARD_H_

#include <cstdint>

namespace chaos::il2cpp::jit {

/// RAII guard for the JIT code registry spinlock.
///
/// Usage:
///   void WinSehHandler::RegisterCode(...) noexcept {
///       JitRegistryLockGuard lock(this);
///       // ... locked operations ...
///   }  // lock auto-released
///
/// @tparam Handler  The handler class (WinSehHandler or LinuxSehHandler).
///                  Must provide AcquireLock() and ReleaseLock() methods.
template <typename Handler>
class JitRegistryLockGuard {
public:
    explicit JitRegistryLockGuard(Handler* handler) noexcept
        : handler_(handler) {
        handler_->AcquireLock();
    }

    ~JitRegistryLockGuard() noexcept {
        handler_->ReleaseLock();
    }

    JitRegistryLockGuard(const JitRegistryLockGuard&) = delete;
    JitRegistryLockGuard& operator=(const JitRegistryLockGuard&) = delete;

private:
    Handler* handler_;
};

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_JIT_REGISTRY_LOCK_GUARD_H_
