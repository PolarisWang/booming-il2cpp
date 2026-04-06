#include "runtime_abi.h"

namespace chaos::contracts::native::examples {

void ReflectionHandlesExample(
    RuntimeState* runtime,
    ThreadState* thread,
    AssemblyHandle assembly,
    void* instance) {
    const RuntimeAbiV0* abi = chaos_runtime_get_abi_v0();
    if (abi == nullptr) {
        return;
    }

    ImageHandle image = abi->assembly_get_image(assembly);
    TypeInfoHandle type = abi->image_find_type(image, "Game.Core", "Player");
    MethodInfoHandle method = abi->type_find_method(type, "TakeDamage", 1);
    FieldInfoHandle field = abi->type_find_field(type, "_health");
    PropertyInfoHandle property = abi->type_find_property(type, "Health");
    EventInfoHandle event_handle = abi->type_find_event(type, "Damaged");
    ParameterInfoHandle parameter = abi->method_get_parameter(method, 0);
    GenericContextHandle generic_context = abi->method_get_generic_context(method);

    int damage = 10;
    int health = 0;
    void* argv[] = {&damage};
    ExceptionHandle exception = nullptr;

    if (field != nullptr) {
        (void)abi->field_get_value(runtime, thread, field, instance, &health, sizeof(health));
    }

    if (method != nullptr) {
        (void)abi->method_invoke(
            runtime,
            thread,
            method,
            instance,
            argv,
            1,
            nullptr,
            0,
            &exception);
    }

    (void)property;
    (void)event_handle;
    (void)parameter;
    (void)generic_context;
}

}  // namespace chaos::contracts::native::examples

