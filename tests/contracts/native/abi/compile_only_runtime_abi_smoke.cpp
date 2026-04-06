#include "runtime_abi.h"

#include <type_traits>

namespace {

void CompileOnlyRuntimeAbiSmoke(const RuntimeAbiV0* abi) {
    RuntimeInitParams init_params = {};
    init_params.struct_size = sizeof(init_params);
    init_params.init_flags = CHAOS_RUNTIME_INIT_ENABLE_LOGGING | CHAOS_RUNTIME_INIT_ENABLE_PROFILING;
    init_params.host_name_utf8 = "compile-only";
    init_params.runtime_tag_utf8 = "abi-v0";

    RuntimeConfig config = {};
    config.struct_size = sizeof(config);

    RuntimeState* runtime = nullptr;
    ThreadState* thread = nullptr;
    TypeInfoHandle type = nullptr;
    MethodInfoHandle method = nullptr;
    FieldInfoHandle field = nullptr;
    PropertyInfoHandle property = nullptr;
    EventInfoHandle event_handle = nullptr;
    TypeInfoHandle generic_type_definition = nullptr;
    ParameterInfoHandle parameter = nullptr;
    GenericContextHandle generic_context = nullptr;
    AssemblyHandle assembly = nullptr;
    ImageHandle image = nullptr;
    ExceptionHandle exception = nullptr;

    int input_value = 7;
    int output_value = 0;
    void* argv[] = {&input_value};

    if (abi == nullptr) {
        return;
    }

    (void)abi->runtime_init(&init_params, &config, &runtime);
    (void)abi->thread_attach(runtime, &thread);

    (void)abi->object_new(runtime, thread, type);
    (void)abi->array_new(runtime, thread, type, 4u);
    (void)abi->string_new_utf8(runtime, thread, "player", 6u);
    (void)abi->class_init(runtime, type);

    GCHandle gc_handle = abi->gc_handle_new(runtime, nullptr, false);
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
    generic_type_definition = abi->type_get_generic_type_definition(type);
    parameter = abi->method_get_parameter(method, 0);
    generic_context = abi->method_get_generic_context(method);

    (void)property;
    (void)event_handle;
    (void)generic_type_definition;
    (void)parameter;
    (void)generic_context;

    abi->thread_detach(runtime, thread);
    abi->runtime_shutdown(runtime);
}

static_assert(CHAOS_RUNTIME_ABI_V0 == 0u, "runtime ABI version must remain v0");
static_assert(std::is_same<RuntimeStatus, int32_t>::value, "runtime status must stay int32_t");
static_assert(std::is_same<GCHandle, uint64_t>::value, "GC handle type drifted");

}  // namespace

