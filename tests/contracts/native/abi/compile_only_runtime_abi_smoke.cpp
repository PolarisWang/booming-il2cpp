#include "boom_runtime_abi.h"

#include <type_traits>

namespace {

void CompileOnlyRuntimeAbiSmoke(const BoomRuntimeAbiV0* abi) {
    BoomRuntimeInitParams init_params = {};
    init_params.struct_size = sizeof(init_params);
    init_params.init_flags = BOOM_RUNTIME_INIT_ENABLE_LOGGING | BOOM_RUNTIME_INIT_ENABLE_PROFILING;
    init_params.host_name_utf8 = "compile-only";
    init_params.runtime_tag_utf8 = "abi-v0";

    BoomRuntimeConfig config = {};
    config.struct_size = sizeof(config);

    BoomRuntimeState* runtime = nullptr;
    BoomThreadState* thread = nullptr;
    BoomTypeInfoHandle type = nullptr;
    BoomMethodInfoHandle method = nullptr;
    BoomFieldInfoHandle field = nullptr;
    BoomPropertyInfoHandle property = nullptr;
    BoomEventInfoHandle event_handle = nullptr;
    BoomParameterInfoHandle parameter = nullptr;
    BoomGenericContextHandle generic_context = nullptr;
    BoomAssemblyHandle assembly = nullptr;
    BoomImageHandle image = nullptr;
    BoomExceptionHandle exception = nullptr;

    int input_value = 7;
    int output_value = 0;
    void* argv[] = {&input_value};

    if (abi == nullptr) {
        return;
    }

    (void)abi->runtime_init(&init_params, &config, &runtime);
    abi->runtime_shutdown(runtime);

    (void)abi->thread_attach(runtime, &thread);
    abi->thread_detach(runtime, thread);

    (void)abi->object_new(runtime, thread, type);
    (void)abi->array_new(runtime, thread, type, 4u);
    (void)abi->string_new_utf8(runtime, thread, "player", 6u);
    (void)abi->class_init(runtime, type);

    BoomGCHandle gc_handle = abi->gc_handle_new(runtime, nullptr, false);
    abi->gc_handle_free(runtime, gc_handle);
    abi->raise_managed_exception(runtime, thread, exception);

    (void)abi->field_get_value(runtime, thread, field, nullptr, &output_value, sizeof(output_value));
    (void)abi->field_set_value(runtime, thread, field, nullptr, &input_value, sizeof(input_value));
    (void)abi->method_invoke(
        runtime,
        thread,
        method,
        nullptr,
        argv,
        1,
        &output_value,
        sizeof(output_value),
        &exception);

    image = abi->assembly_get_image(assembly);
    type = abi->image_find_type(image, "Game.Core", "Player");
    method = abi->type_find_method(type, "TakeDamage", 1);
    field = abi->type_find_field(type, "_health");
    property = abi->type_find_property(type, "Health");
    event_handle = abi->type_find_event(type, "Damaged");
    parameter = abi->method_get_parameter(method, 0);
    generic_context = abi->method_get_generic_context(method);

    (void)property;
    (void)event_handle;
    (void)parameter;
    (void)generic_context;
}

static_assert(BOOM_RUNTIME_ABI_V0 == 0u, "runtime ABI version must remain v0");
static_assert(std::is_same<BoomGCHandle, uint64_t>::value, "GC handle type drifted");

}  // namespace
