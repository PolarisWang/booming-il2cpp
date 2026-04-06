#ifndef CHAOS_IL2CPP_BOOTSTRAP_H_
#define CHAOS_IL2CPP_BOOTSTRAP_H_

#include "codegen_bridge.h"

namespace chaos::il2cpp::bootstrap {

struct BootstrapState {
    const CodeRegistrationV0* code_registration;
    const MetadataRegistrationV0* metadata_registration;
    const CodegenRegistrationOptionsV0* options;
    bool is_registered;
    bool is_bootstrapped;
};

const BootstrapState* PeekBootstrapState();
const CodegenBridgeV0* GetCodegenBridgeV0();

}  // namespace chaos::il2cpp::bootstrap

#endif  // CHAOS_IL2CPP_BOOTSTRAP_H_
