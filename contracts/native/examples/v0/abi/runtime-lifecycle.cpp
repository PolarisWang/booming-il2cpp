#include "boom_runtime_abi.h"

namespace boom::contracts::native::examples {

void RuntimeLifecycleExample() {
    const BoomRuntimeAbiV0* abi = boom_runtime_get_abi_v0();
    if (abi == nullptr) {
        return;
    }

    BoomRuntimeInitParams init_params = {};
    init_params.struct_size = sizeof(init_params);
    init_params.init_flags = BOOM_RUNTIME_INIT_ENABLE_LOGGING;
    init_params.host_name_utf8 = "roadmap0-smoke";
    init_params.runtime_tag_utf8 = "native-contract-v0";

    BoomRuntimeConfig config = {};
    config.struct_size = sizeof(config);

    BoomRuntimeState* runtime = nullptr;
    BoomThreadState* thread = nullptr;

    if (abi->runtime_init(&init_params, &config, &runtime) != BOOM_RUNTIME_STATUS_OK) {
        return;
    }

    if (abi->thread_attach(runtime, &thread) == BOOM_RUNTIME_STATUS_OK) {
        abi->thread_detach(runtime, thread);
    }

    abi->runtime_shutdown(runtime);
}

}  // namespace boom::contracts::native::examples
