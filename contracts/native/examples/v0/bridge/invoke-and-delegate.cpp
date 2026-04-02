#include "boom_codegen_bridge.h"

namespace boom::contracts::native::examples {

void InvokeAndDelegateExample(
    BoomRuntimeState* runtime,
    BoomThreadState* thread,
    BoomTypeInfoHandle instance_type,
    BoomMethodInfoHandle declared_method,
    void* instance) {
    const BoomCodegenBridgeV0* bridge = boom_codegen_get_bridge_v0();
    if (bridge == nullptr) {
        return;
    }

    int argument = 42;
    int return_value = 0;
    void* argv[] = {&argument};
    BoomExceptionHandle exception = nullptr;

    BoomMethodInfoHandle virtual_target =
        bridge->resolve_virtual_method(instance_type, declared_method);
    if (virtual_target != nullptr) {
        (void)bridge->invoke_virtual(
            runtime,
            thread,
            instance,
            virtual_target,
            argv,
            1,
            &return_value,
            sizeof(return_value),
            &exception);
    }

    void* delegate_instance = bridge->create_delegate(runtime, thread, declared_method, instance);
    if (delegate_instance != nullptr) {
        (void)bridge->delegate_invoke(
            runtime,
            thread,
            delegate_instance,
            argv,
            1,
            &return_value,
            sizeof(return_value),
            &exception);
    }

    void* icall = bridge->resolve_icall("Boom.Runtime.InternalCalls::Tick");
    (void)icall;
}

}  // namespace boom::contracts::native::examples
