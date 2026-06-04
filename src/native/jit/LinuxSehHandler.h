#ifndef CHAOS_IL2CPP_LINUXSEHHANDLER_H_
#define CHAOS_IL2CPP_LINUXSEHHANDLER_H_

#include "ISehHandler.h"
#include "jit_method.h"

#include <atomic>
#include <cstdint>

namespace chaos::il2cpp::jit {

/// POSIX signal-based SEH handler for T4-generated native code on Linux.
///
/// Implements the ISehHandler interface using POSIX signal handlers
/// (SIGSEGV/SIGBUS for hardware exceptions, SIGUSR1 for managed throws).
///
/// Thread safety:
///   - The T4 code registry (entries_[]) is protected by a spinlock (lock_).
///   - The TLS lookup cache provides O(1) repeated lookups.
///   - A global generation counter invalidates stale caches on demotion.
class LinuxSehHandler : public ISehHandler {
public:
    LinuxSehHandler() = default;
    ~LinuxSehHandler() noexcept override;

    // ISehHandler interface
    void RegisterCode(void* code_start, uint32_t code_size,
                      const JitMethod* nm,
                      uint32_t patch_method_token = 0) noexcept override;
    void UnregisterCode(void* code_start) noexcept override;
    const JitMethod* FindCodeByAddress(const void* address) noexcept override;
    uint32_t DemoteByToken(uint32_t method_token) noexcept override;
    uint32_t DemoteByDomainId(uint32_t domain_id) noexcept override;
    uint32_t DemoteByCallSiteToken(uint32_t method_token) noexcept override;
    void ReclaimDemoted() noexcept override;
    void Initialize() noexcept override;

private:
    // ── T4 Code Registry ──────────────────────────────────────────────────
    static constexpr uint32_t kMaxJitCodeEntries = 2048;

    struct JitCodeEntry {
        const void*       code_start = nullptr;
        uint32_t          code_size  = 0;
        const JitMethod*  nm         = nullptr;
        uint32_t          patch_method_token = 0;
        uint32_t          domain_id  = 0;  // 0 = core domain (never unloaded)
    };

    JitCodeEntry entries_[kMaxJitCodeEntries];
    uint32_t    count_ = 0;
    std::atomic<long> lock_{0};

    // RAII guard is a friend so it can call AcquireLock/ReleaseLock.
    template <typename T>
    friend class JitRegistryLockGuard;

    // ── Pending Free Regions ────────────────────────────────────────────
    static constexpr uint32_t kMaxPendingFreeRegions = 64;

    struct PendingFreeRegion {
        void*    code_start = nullptr;
        uint32_t code_size  = 0;
        bool     active     = false;
    };

    PendingFreeRegion pending_free_[kMaxPendingFreeRegions];
    uint32_t          pending_free_count_ = 0;

    // ── Lookup Cache Generation ─────────────────────────────────────────
    std::atomic<uint32_t> lookup_generation_{1};

    void AcquireLock() noexcept;
    void ReleaseLock() noexcept;
    void EnqueueDemotedCode(void* code_start, uint32_t code_size) noexcept;
    void InvalidateLookupCache() noexcept;
};

/// Platform-agnostic singleton accessor.
/// Returns WinSehHandler on Windows, LinuxSehHandler on Linux.
ISehHandler& GetSehHandler() noexcept;

// Convenience alias for backward compatibility during transition.
#if defined(_WIN32) || defined(_WIN64)
// On Windows, GetSehHandler returns WinSehHandler (defined in WinSehHandler.h)
#else
// On Linux, use LinuxSehHandler
inline LinuxSehHandler& GetLinuxSehHandler() noexcept {
    static LinuxSehHandler instance;
    return instance;
}
#endif

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_LINUXSEHHANDLER_H_
