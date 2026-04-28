#include "codegen_bridge.h"

#include <type_traits>

namespace {

void CompileOnlyCodegenBridgeSmoke(const CodegenBridgeV0* bridge) {
    CodeRegistrationV0 code_registration = {};
    code_registration.struct_size = sizeof(code_registration);
    code_registration.type_capabilities = nullptr;
    code_registration.type_capability_count = 0u;

    MetadataRegistrationV0 metadata_registration = {};
    metadata_registration.struct_size = sizeof(metadata_registration);

    CodegenRegistrationOptionsV0 options = {};
    options.struct_size = sizeof(options);
    options.image_name_utf8 = "GameAssembly";

    RuntimeState* runtime = nullptr;
    ThreadState* thread = nullptr;
    ImageHandle image = nullptr;
    TypeInfoHandle type = nullptr;
    MethodInfoHandle method = nullptr;
    FieldInfoHandle field = nullptr;
    ExceptionHandle exception = nullptr;
    RuntimeTypeCapabilityInfoV0 capability_info = {};

    int input_value = 3;
    int output_value = 0;
    void* argv[] = {&input_value};

    if (bridge == nullptr) {
        return;
    }

    (void)bridge->register_codegen(&code_registration, &metadata_registration, &options);
    (void)bridge->bootstrap_runtime();

    type = bridge->resolve_type_by_token(image, 0x02000001u);
    method = bridge->resolve_method_by_token(image, 0x06000001u);
    field = bridge->resolve_field_by_token(image, 0x04000001u);

    (void)bridge->box_value(runtime, thread, type, &input_value, sizeof(input_value));
    (void)bridge->unbox_value(runtime, nullptr, &output_value, sizeof(output_value));

    method = bridge->resolve_virtual_method(type, method);
    (void)bridge->invoke_virtual(
        runtime,
        thread,
        nullptr,
        method,
        argv,
        1,
        &output_value,
        sizeof(output_value),
        &exception);

    void* delegate_instance = bridge->create_delegate(runtime, thread, method, nullptr);
    (void)bridge->delegate_invoke(
        runtime,
        thread,
        delegate_instance,
        argv,
        1,
        &output_value,
        sizeof(output_value),
        &exception);

    (void)bridge->resolve_icall("Chaos.Runtime.InternalCalls::Tick");
    capability_info.struct_size = sizeof(capability_info);
    (void)bridge->query_type_capability(type, &capability_info);
    (void)field;
}

static_assert(CHAOS_CODEGEN_BRIDGE_V0 == 0u, "bridge version must remain v0");
static_assert(std::is_same<BridgeStatus, int32_t>::value, "bridge status must stay int32_t");
static_assert(CHAOS_BRIDGE_STATUS_NOT_SUPPORTED == 4, "bridge not-supported status must remain stable");
static_assert(CHAOS_BRIDGE_STATUS_MANAGED_EXCEPTION == 5, "bridge managed-exception status must remain stable");
static_assert(CHAOS_BRIDGE_STATUS_INTERNAL_ERROR == 6, "bridge internal-error status must remain stable");
static_assert(CHAOS_BRIDGE_STATUS_METADATA_RESOLUTION_FAILED == 7, "bridge metadata-resolution-failed status must remain stable");
static_assert(CHAOS_BRIDGE_STATUS_CLASS_INIT_FAILED == 8, "bridge class-init-failed status must remain stable");
static_assert(CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED == 9, "bridge runtime-call-failed status must remain stable");

}  // namespace

