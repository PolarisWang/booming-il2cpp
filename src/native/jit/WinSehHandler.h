#ifndef CHAOS_IL2CPP_WINSEHHANDLER_H_
#define CHAOS_IL2CPP_WINSEHHANDLER_H_

#include "ISehHandler.h"
#include "jit_method.h"

#include <atomic>
#include <cstdint>
#include <vector>

namespace chaos::il2cpp::jit {

/// Windows VEH-based SEH handler for T4-generated native code.
///
/// Implements the ISehHandler interface using Windows Vectored Exception
/// Handling (VEH).  On Windows, this is the primary mechanism for dispatching
/// both managed exceptions (throws from T4-generated code) and hardware
/// exceptions (AV, div-by-zero, etc.) through the SEH clause table embedded
/// in each T4 method's code buffer.
///
/// Thread safety:
///   - The T4 code registry (entries_[]) is protected by a spinlock (lock_).
///   - The TLS lookup cache (g_jit_lookup_cache, not in class) provides O(1)
///     repeated lookups within the same code page.
///   - A global generation counter (lookup_generation_) invalidates stale
///     TLS caches across threads when code is demoted.
class WinSehHandler : public ISehHandler {
public:
    WinSehHandler() = default;
    ~WinSehHandler() noexcept override;

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
    // Maps code address ranges back to JitMethod for VEH lookup.
    // Dynamic vector (no fixed cap; reserve 4096 at init).
    // Thread-safe: entries are append-only, never removed.

    struct JitCodeEntry {
        const void*       code_start = nullptr;   // RX code entry point
        uint32_t          code_size  = 0;          // bytes
        const JitMethod*  nm         = nullptr;
        uint32_t          patch_method_token = 0;  // PatchMethod token for hotpatch demotion
        uint32_t          domain_id  = 0;           // 0 = core domain (never unloaded)
    };

    std::vector<JitCodeEntry> entries_;
    std::atomic<long> lock_{0};  // spinlock: 0=free, 1=locked

    // RAII guard is a friend so it can call AcquireLock/ReleaseLock.
    template <typename T>
    friend class JitRegistryLockGuard;

    // ── Pending Free Regions ────────────────────────────────────────────
    // When T4 code is demoted, we defer VirtualFree to the next GC safepoint.
    // Dynamic vector (reserve 256 at init).

    struct PendingFreeRegion {
        void*    code_start = nullptr;
        uint32_t code_size  = 0;
        bool     active     = false;
    };

    std::vector<PendingFreeRegion> pending_free_;

    // ── Lookup Cache Generation ─────────────────────────────────────────
    // Global generation counter for TLS lookup cache invalidation.
    // Each time T4 code is demoted/unregistered, this counter is incremented.
    std::atomic<uint32_t> lookup_generation_{1};  // 0 is reserved for "not initialized"

    // ── Spinlock helpers ─────────────────────────────────────────────────
    void AcquireLock() noexcept;
    void ReleaseLock() noexcept;

    // ── Demotion helpers ─────────────────────────────────────────────────
    void EnqueueDemotedCode(void* code_start, uint32_t code_size) noexcept;
    void InvalidateLookupCache() noexcept;
};

/// Global singleton accessor for use by extern "C" functions and generated code.
/// The singleton is created on first call and lives for the process lifetime.
WinSehHandler& GetWinSehHandler() noexcept;

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_WINSEHHANDLER_H_
