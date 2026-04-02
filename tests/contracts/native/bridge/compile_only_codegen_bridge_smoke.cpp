#include "boom_codegen_bridge.h"

#include <type_traits>

namespace {

void CompileOnlyCodegenBridgeSmoke(const BoomCodegenBridgeV0* bridge) {
    BoomCodeRegistrationV0 code_registration = {};
    code_registration.struct_size = sizeof(code_registration);

    BoomMetadataRegistrationV0 metadata_registration = {};
    metadata_registration.struct_size = sizeof(metadata_registration);

    BoomCodegenRegistrationOptionsV0 options = {};
    options.struct_size = sizeof(options);
    options.image_name_utf8 = "GameAssembly";

    BoomRuntimeState* runtime = nullptr;
    BoomThreadState* thread = nullptr;
    BoomImageHandle image = nullptr;
    BoomTypeInfoHandle type = nullptr;
    BoomMethodInfoHandle method = nullptr;
    BoomFieldInfoHandle field = nullptr;
    BoomExceptionHandle exception = nullptr;

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

    (void)bridge->resolve_icall("Boom.Runtime.InternalCalls::Tick");
    (void)field;
}

static_assert(BOOM_CODEGEN_BRIDGE_V0 == 0u, "bridge version must remain v0");
static_assert(std::is_enum<BoomBridgeStatus>::value, "bridge status must stay an enum");

}  // namespace
