#ifndef CHAOS_IL2CPP_ISEHHANDLER_H_
#define CHAOS_IL2CPP_ISEHHANDLER_H_

#include <cstdint>

namespace chaos::il2cpp::jit {

struct JitMethod;

class ISehHandler {
public:
    virtual ~ISehHandler() = default;

    /// Register a range of T4-generated code for VEH/signal lookup.
    virtual void RegisterCode(void* code_start, uint32_t code_size,
                              const JitMethod* nm,
                              uint32_t patch_method_token = 0) noexcept = 0;

    /// Unregister a T4 code range.
    virtual void UnregisterCode(void* code_start) noexcept = 0;

    /// Find the JitMethod covering a given code address.
    virtual const JitMethod* FindCodeByAddress(const void* address) noexcept = 0;

    /// Demote all T4 code entries matching the given method_token.
    virtual uint32_t DemoteByToken(uint32_t method_token) noexcept = 0;

    /// Demote all T4 code entries whose call_sites reference the given method_token.
    virtual uint32_t DemoteByCallSiteToken(uint32_t method_token) noexcept = 0;

    /// Free all demoted T4 code regions deferred to next GC safepoint.
    virtual void ReclaimDemoted() noexcept = 0;

    /// Initialize the handler (register VEH handler / signal handler).
    virtual void Initialize() noexcept = 0;
};

/// Platform-agnostic singleton accessor.
/// Returns WinSehHandler on Windows, LinuxSehHandler on Linux.
ISehHandler& GetSehHandler() noexcept;

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_ISEHHANDLER_H_
