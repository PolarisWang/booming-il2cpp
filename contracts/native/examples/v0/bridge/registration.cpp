#include "boom_codegen_bridge.h"

namespace boom::contracts::native::examples {

void RegistrationExample() {
    const BoomCodegenBridgeV0* bridge = boom_codegen_get_bridge_v0();
    if (bridge == nullptr) {
        return;
    }

    BoomCodeRegistrationV0 code_registration = {};
    code_registration.struct_size = sizeof(code_registration);

    BoomMetadataRegistrationV0 metadata_registration = {};
    metadata_registration.struct_size = sizeof(metadata_registration);

    BoomCodegenRegistrationOptionsV0 options = {};
    options.struct_size = sizeof(options);
    options.image_name_utf8 = "GameAssembly";

    if (bridge->register_codegen(&code_registration, &metadata_registration, &options) !=
        BOOM_BRIDGE_STATUS_OK) {
        return;
    }

    (void)bridge->bootstrap_runtime();
}

}  // namespace boom::contracts::native::examples
