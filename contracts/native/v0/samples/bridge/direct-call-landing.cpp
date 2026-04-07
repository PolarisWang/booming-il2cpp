#include "codegen_bridge.h"

namespace chaos::contracts::native::examples {

extern "C" int32_t CHAOS_RUNTIME_ABI_CALL HelloWorldObject_Program_Main(
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args);

extern "C" void CHAOS_RUNTIME_ABI_CALL HelloWorldObject_Greeter__ctor(
    RuntimeState* runtime,
    ThreadState* thread,
    void* greeter_instance,
    void* name_string);

extern "C" void* CHAOS_RUNTIME_ABI_CALL HelloWorldObject_Greeter_BuildMessage(
    RuntimeState* runtime,
    ThreadState* thread,
    void* greeter_instance);

void DirectCallLandingExample(
    RuntimeState* runtime,
    ThreadState* thread,
    ImageHandle image,
    void* managed_args) {
    const RuntimeAbiV0* abi = chaos_runtime_get_abi_v0();
    const CodegenBridgeV0* bridge = chaos_codegen_get_bridge_v0();
    if (abi == nullptr || bridge == nullptr) {
        return;
    }

    CodeRegistrationV0 code_registration = {};
    code_registration.struct_size = sizeof(code_registration);

    MetadataRegistrationV0 metadata_registration = {};
    metadata_registration.struct_size = sizeof(metadata_registration);

    CodegenRegistrationOptionsV0 options = {};
    options.struct_size = sizeof(options);
    options.image_name_utf8 = "HelloWorldObject";

    if (bridge->register_codegen(&code_registration, &metadata_registration, &options) !=
        CHAOS_BRIDGE_STATUS_OK) {
        return;
    }

    if (bridge->bootstrap_runtime() != CHAOS_BRIDGE_STATUS_OK) {
        return;
    }

    TypeInfoHandle greeter_type = bridge->resolve_type_by_token(image, 0x02000002u);
    (void)bridge->resolve_method_by_token(image, 0x06000001u);
    (void)bridge->resolve_method_by_token(image, 0x06000002u);
    (void)bridge->resolve_method_by_token(image, 0x06000003u);
    (void)bridge->resolve_field_by_token(image, 0x04000001u);

    void* world = abi->string_new_utf8(runtime, thread, "World", 5u);
    void* greeter = abi->object_new(runtime, thread, greeter_type);
    if (world == nullptr || greeter == nullptr) {
        return;
    }

    // first proof requires generated direct-call landing, not method_invoke fallback.
    HelloWorldObject_Greeter__ctor(runtime, thread, greeter, world);
    void* message = HelloWorldObject_Greeter_BuildMessage(runtime, thread, greeter);
    (void)message;

    (void)HelloWorldObject_Program_Main(runtime, thread, managed_args);
}

}  // namespace chaos::contracts::native::examples

