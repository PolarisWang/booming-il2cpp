#include "runtime_abi.h"

namespace chaos::contracts::native::examples {

void RuntimeLifecycleExample() {
    const RuntimeAbiV0* abi = chaos_runtime_get_abi_v0();
    if (abi == nullptr) {
        return;
    }

    RuntimeInitParams init_params = {};
    init_params.struct_size = sizeof(init_params);
    init_params.init_flags = CHAOS_RUNTIME_INIT_ENABLE_LOGGING;
    init_params.host_name_utf8 = "roadmap0-smoke";
    init_params.runtime_tag_utf8 = "native-contract-v0";

    RuntimeConfig config = {};
    config.struct_size = sizeof(config);

    RuntimeState* runtime = nullptr;
    ThreadState* thread = nullptr;

    if (abi->runtime_init(&init_params, &config, &runtime) != CHAOS_RUNTIME_STATUS_OK) {
        return;
    }

    if (abi->thread_attach(runtime, &thread) == CHAOS_RUNTIME_STATUS_OK) {
        abi->thread_detach(runtime, thread);
    }

    abi->runtime_shutdown(runtime);
}

}  // namespace chaos::contracts::native::examples

