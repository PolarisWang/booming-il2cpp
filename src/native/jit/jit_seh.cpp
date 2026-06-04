// jit_seh.cpp — Platform-agnostic forwarding layer for T4 SEH dispatch.
//
// Each function delegates to the platform-appropriate ISehHandler singleton
// obtained via GetSehHandler().  On Windows this resolves to WinSehHandler
// (VEH-based); on Linux it resolves to LinuxSehHandler (signal-based).

#include "jit_seh.h"
#include "ISehHandler.h"
#include "jit_method.h"

#include <chaos/log.h>

// Register demotion callbacks with runtime_core so method_replacement
// can demote T4 entries without creating a circular dependency.
#include <jit_demotion.h>

// GC event callbacks for deferred T4 code memory reclamation.
#include <gc_events.h>

// Memory domain events: DOMAIN_UNLOADED triggers JIT code demotion
// for the unloaded domain's methods.
#include <memory_domain_events.h>

// JIT debug contract (for SOS extension — no-op if contract not linked).
#include "jit/jit_debug_contract.h"

// MetadataRegistry for debug contract.
#include "metadata_interface.h"

// ── Forwarding functions ───────────────────────────────────────────────────
//
// These free functions are the public API for jit_seh.h.  Existing callers
// (jit_helpers.cpp, entry_direct.cpp, runtime_init.cpp, fast_dispatch.cpp,
// osr_trigger.cpp, ir_reg_alloc.cpp, thread_state.cpp, method_replacement.cpp,
// bootstrap.cpp, etc.) call them by name.  Each function delegates to the
// global WinSehHandler singleton.
//
// This layer decouples the interface from the implementation: callers don't
// need to know about WinSehHandler directly, and the implementation can be
// replaced with a different ISehHandler (e.g. a POSIX signal handler) without
// touching callers.
//
// TLS variables (g_jit_exception_obj, g_jit_throw_ret_addr, g_jit_frame_rsp,
// g_jit_unwind) are defined in WinSehHandler.cpp and declared extern in
// jit_seh.h.

namespace chaos::il2cpp::jit {

void RegisterNativeCodeSection(void* code_start, uint32_t code_size,
                    const JitMethod* nm,
                    uint32_t patch_method_token) noexcept {
    GetSehHandler().RegisterCode(code_start, code_size, nm, patch_method_token);

    // Sync debug mirror for SOS extension.
    // Must follow the real registration so the mirror matches runtime state.
    JitDebugContractAddEntry(code_start, code_size, nm, patch_method_token);
}

void UnregisterNativeCodeSection(void* code_start) noexcept {
    GetSehHandler().UnregisterCode(code_start);
}

const JitMethod* FindNativeCodeByAddress(const void* address) noexcept {
    return GetSehHandler().FindCodeByAddress(address);
}

uint32_t DemoteJittedMethod(uint32_t method_token) noexcept {
    return GetSehHandler().DemoteByToken(method_token);
}

uint32_t DemoteJittedCodeByDomain(uint32_t domain_id) noexcept {
    return GetSehHandler().DemoteByDomainId(domain_id);
}

uint32_t DemoteJittedCallSite(uint32_t method_token) noexcept {
    return GetSehHandler().DemoteByCallSiteToken(method_token);
}

void ReclaimDemotedCode() noexcept {
    GetSehHandler().ReclaimDemoted();
}

void RegisterJitSehHandler() noexcept {
    GetSehHandler().Initialize();

    // Register DOMAIN_UNLOADED callback to demote JIT code belonging to
    // the unloaded domain.  This prevents the VEH/signal handler from
    // dispatching into JIT code that references freed domain metadata.
    // Safe during STW: ReclaimDemoted is called inline.
    memory_domain::MemoryDomainRegisterEventCallback(
        [](memory_domain::MemoryDomainEvent event,
           const memory_domain::MemoryDomainEventData* data,
           void* /*user_data*/) noexcept {
            if (event == memory_domain::MemoryDomainEvent::DOMAIN_UNLOADED) {
                uint32_t demoted = GetSehHandler().DemoteByDomainId(data->domain_id);
                if (demoted > 0) {
                    // Inline reclamation is safe during STW (domain unload
                    // holds the GC safepoint while the callback fires).
                    GetSehHandler().ReclaimDemoted();
                }
            }
        }, nullptr);

    // Prime the debug contract metadata registry pointer so SOS extension
    // can resolve method names via the unified registry interface.
    JitDebugContractInitMetadataRegistry(
        runtime_core::MetadataRegistry::Get().GetUnifiedRegistry());
}

}  // namespace chaos::il2cpp::jit
