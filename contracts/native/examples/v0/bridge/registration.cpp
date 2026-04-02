#include "codegen_bridge.h"

namespace boom::contracts::native::examples {

void RegistrationExample() {
    const CodegenBridgeV0* bridge = boom_codegen_get_bridge_v0();
    if (bridge == nullptr) {
        return;
    }

    CodeRegistrationV0 code_registration = {};
    code_registration.struct_size = sizeof(code_registration);

    MetadataRegistrationV0 metadata_registration = {};
    metadata_registration.struct_size = sizeof(metadata_registration);

    CodegenRegistrationOptionsV0 options = {};
    options.struct_size = sizeof(options);
    options.image_name_utf8 = "GameAssembly";

    if (bridge->register_codegen(&code_registration, &metadata_registration, &options) !=
        BOOM_BRIDGE_STATUS_OK) {
        return;
    }

    (void)bridge->bootstrap_runtime();
}

}  // namespace boom::contracts::native::examples
