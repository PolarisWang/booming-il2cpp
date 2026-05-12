#ifndef CHAOS_IL2CPP_DOMAIN_UNLOADER_H_
#define CHAOS_IL2CPP_DOMAIN_UNLOADER_H_

#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_core {

// ======================================================================
// Domain unloader — safe stop-the-world domain teardown
//
// Orchestrates: safepoint → scan cross-domain refs → clear refs →
// release regions → unregister memory domain → resume threads.
// ======================================================================

struct DomainUnloadResult {
    CHAOS_IL2CPP_SIZE cross_domain_refs_found;
    CHAOS_IL2CPP_SIZE cross_domain_refs_cleared;
    bool              success;
};

/// Unload all regions and memory for the given domain.
/// Must NOT be called from within a GC safepoint (request/release is internal).
/// @param domain_id  The domain to unload.
DomainUnloadResult UnloadDomain(CHAOS_IL2CPP_UINT32 domain_id);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_DOMAIN_UNLOADER_H_