#ifndef CHAOS_IL2CPP_BOOTSTRAP_H_
#define CHAOS_IL2CPP_BOOTSTRAP_H_

#include "codegen_bridge.h"
#include "memory_domain.h"

#include <cstdint>

namespace chaos::il2cpp::bootstrap {

struct BootstrapState {
    const CodeRegistrationV0* code_registration;
    const MetadataRegistrationV0* metadata_registration;
    const CodegenRegistrationOptionsV0* options;
    CHAOS_IL2CPP_UINT32 aot_domain_id;          ///< MemoryDomain for the AOT root module (0 = not registered).
    bool is_registered;
    bool is_bootstrapped;
};

const BootstrapState* PeekBootstrapState();
const CodegenBridgeV0* GetCodegenBridgeV0();
const RuntimeTypeCapabilityEntryV0* FindRegisteredTypeCapabilityEntry(CHAOS_IL2CPP_UINT32 type_token);
void* FindInvokerPointer(CHAOS_IL2CPP_UINT32 method_token);
void* FindMethodPointerByToken(CHAOS_IL2CPP_UINT32 method_token);
memory_domain::DomainId GetAotDomainId();

}  // namespace chaos::il2cpp::bootstrap

#endif  // CHAOS_IL2CPP_BOOTSTRAP_H_
