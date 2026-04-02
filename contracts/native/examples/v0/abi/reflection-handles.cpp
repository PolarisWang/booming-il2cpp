#include "boom_runtime_abi.h"

namespace boom::contracts::native::examples {

void ReflectionHandlesExample(
    BoomRuntimeState* runtime,
    BoomThreadState* thread,
    BoomAssemblyHandle assembly,
    void* instance) {
    const BoomRuntimeAbiV0* abi = boom_runtime_get_abi_v0();
    if (abi == nullptr) {
        return;
    }

    BoomImageHandle image = abi->assembly_get_image(assembly);
    BoomTypeInfoHandle type = abi->image_find_type(image, "Game.Core", "Player");
    BoomMethodInfoHandle method = abi->type_find_method(type, "TakeDamage", 1);
    BoomFieldInfoHandle field = abi->type_find_field(type, "_health");
    BoomPropertyInfoHandle property = abi->type_find_property(type, "Health");
    BoomEventInfoHandle event_handle = abi->type_find_event(type, "Damaged");
    BoomParameterInfoHandle parameter = abi->method_get_parameter(method, 0);
    BoomGenericContextHandle generic_context = abi->method_get_generic_context(method);

    int damage = 10;
    int health = 0;
    void* argv[] = {&damage};
    BoomExceptionHandle exception = nullptr;

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

}  // namespace boom::contracts::native::examples
