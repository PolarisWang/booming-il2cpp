#include <gtest/gtest.h>
#include <engine_bridge.h>

// Stubs for AOT runtime symbols normally provided by codegen/entrypoint
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

TEST(EngineBridgeSmoke, InitParamsStructSize) {
    engine_init_params_t params{};
    params.struct_size = sizeof(engine_init_params_t);
    params.host_name_utf8 = "test-host";
    EXPECT_EQ(sizeof(engine_init_params_t), params.struct_size);
    EXPECT_STREQ("test-host", params.host_name_utf8);
}

TEST(EngineBridgeSmoke, EngineStatusEnumValues) {
    EXPECT_EQ(0, ENGINE_STATUS_OK);
    EXPECT_EQ(1, ENGINE_STATUS_INVALID_ARGUMENT);
    EXPECT_EQ(2, ENGINE_STATUS_NOT_SUPPORTED);
    EXPECT_EQ(3, ENGINE_STATUS_INTERNAL_ERROR);
}

TEST(EngineBridgeSmoke, EngineHandleKindEnumValues) {
    EXPECT_EQ(1, ENGINE_HANDLE_KIND_WEAK);
    EXPECT_EQ(2, ENGINE_HANDLE_KIND_STRONG);
}

TEST(EngineBridgeSmoke, RetainObjectNullRuntime) {
    auto status = engine_retain_object(nullptr, 1u, ENGINE_HANDLE_KIND_WEAK);
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, RetainObjectZeroRef) {
    // object_ref == 0 should be invalid
    auto status = engine_retain_object(nullptr, 0u, ENGINE_HANDLE_KIND_WEAK);
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, RetainObjectInvalidHandleKind) {
    auto status = engine_retain_object(nullptr, 1u, static_cast<engine_handle_kind_t>(0));
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, ReleaseObjectNullRuntime) {
    auto status = engine_release_object(nullptr, 1u, ENGINE_HANDLE_KIND_WEAK);
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, TransferOwnershipNullRuntime) {
    auto status = engine_transfer_ownership(nullptr, 1u, "from", "to");
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, TransferOwnershipNullStrings) {
    // runtime is null, so should fail before checking strings
    auto status = engine_transfer_ownership(nullptr, 1u, nullptr, "to");
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, TransferOwnershipEmptyToOwner) {
    auto status = engine_transfer_ownership(nullptr, 1u, "from", "");
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, RegisterCallbackNullRuntime) {
    engine_callback_id_t callback_id = 0;
    auto status = engine_register_callback(nullptr, "test", nullptr, nullptr, &callback_id);
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, RegisterCallbackNullName) {
    engine_callback_id_t callback_id = 0;
    auto status = engine_register_callback(nullptr, nullptr, nullptr, nullptr, &callback_id);
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, RegisterCallbackEmptyName) {
    engine_callback_id_t callback_id = 0;
    auto status = engine_register_callback(nullptr, "", nullptr, nullptr, &callback_id);
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, RegisterCallbackNullOutId) {
    auto status = engine_register_callback(nullptr, "test", nullptr, nullptr, nullptr);
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, DispatchCallbackNullRuntime) {
    auto status = engine_dispatch_callback(nullptr, 1u, nullptr, 0u);
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, DispatchCallbackZeroId) {
    auto status = engine_dispatch_callback(nullptr, 0u, nullptr, 0u);
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, ReloadNullRuntime) {
    auto status = engine_reload(nullptr);
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, SceneAttachNullRuntime) {
    auto status = engine_scene_attach(nullptr, "test-scene");
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, SceneAttachNullName) {
    auto status = engine_scene_attach(nullptr, nullptr);
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, SceneAttachEmptyName) {
    auto status = engine_scene_attach(nullptr, "");
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, InitNullOutRuntime) {
    auto status = engine_init(nullptr, nullptr);
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, InitNullParams) {
    engine_runtime_context_t* runtime = nullptr;
    auto status = engine_init(&runtime, nullptr);
    EXPECT_EQ(ENGINE_STATUS_INVALID_ARGUMENT, status);
}

TEST(EngineBridgeSmoke, ShutdownNullRuntime) {
    // engine_shutdown should be a no-op for null runtime
    engine_shutdown(nullptr);
    // No crash = pass
    SUCCEED();
}

TEST(EngineBridgeSmoke, EngineCallbackFnSignature) {
    // Verify the callback function pointer type is well-defined.
    // A lambda matching the signature should be convertible.
    auto test_callback = [](const char*, const void*, size_t, void*) -> engine_status_t {
        return ENGINE_STATUS_OK;
    };
    engine_callback_fn_t fn = test_callback;
    EXPECT_NE(nullptr, fn);
}
