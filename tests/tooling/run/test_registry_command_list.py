from tests.tooling.run.registry_command_test_support import *


class TestRegistryCommandList(RegistryCommandTestSupport):
    def test_registry_list_returns_registered_objects(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_registry_list")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_registry_list")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        result = test_module.handle(
            {"id": "test-registry-list", "handler": "test.dispatch"},
            REPO_ROOT,
            "macos",
            "test registry list",
            manifest,
            {},
        )

        self.assertEqual("ok", result.status)
        flat_ids = {item["id"] for item in result.payload["flatItems"]}
        self.assertIn("contract/analysis-schema", flat_ids)
        self.assertIn("contract/managed-closure-bundle", flat_ids)
        self.assertIn("contract/trace-schema", flat_ids)
        self.assertIn("contract/native-abi", flat_ids)
        self.assertIn("contract/native-bridge", flat_ids)
        self.assertIn("module/analysis/basic", flat_ids)
        self.assertIn("module/managed-closure/basic", flat_ids)
        self.assertIn("module/managed-smoke/basic", flat_ids)
        self.assertIn("module/reflection/basic", flat_ids)
        self.assertIn("module/interop/basic", flat_ids)
        self.assertIn("module/hosted-runtime/basic", flat_ids)
        self.assertIn("module/trace-export/macos", flat_ids)
        self.assertIn("gate/ios-arm64-packaging", flat_ids)
        self.assertIn("gate/linux-x64-packaging", flat_ids)
        self.assertIn("gate/macos-reference-desktop", flat_ids)
        self.assertIn("system/hosted-runtime-smoke", flat_ids)
        self.assertIn("system/ios-packaging-gate", flat_ids)
        self.assertIn("system/linux-packaging-gate", flat_ids)
        self.assertIn("system/macos-reference-gate", flat_ids)
        self.assertIn("system/runtime-baseline-macos", flat_ids)
        self.assertIn("system/trace-export-macos-smoke", flat_ids)
        self.assertNotIn("pipeline/completion-managed-closure", flat_ids)
        self.assertNotIn("pipeline/completion-runtime-core", flat_ids)
        self.assertNotIn("pipeline/completion-runtime-trace-macos", flat_ids)
        self.assertNotIn("pipeline/trace-export-macos-runtime", flat_ids)
        self.assertNotIn("Pipelines:", result.text)
        managed_runtime_suite = select_registry_item(
            result.payload["flatItems"],
            object_type="suite",
            required_module_ids=["managed-smoke"],
            required_supported_hosts=["macos"],
        )
        reflection_suite = select_registry_item(
            result.payload["flatItems"],
            object_type="suite",
            required_module_ids=["reflection"],
            required_supported_hosts=["macos"],
        )
        interop_suite = select_registry_item(
            result.payload["flatItems"],
            object_type="suite",
            required_module_ids=["interop"],
            required_supported_hosts=["macos"],
        )
        hosted_runtime_suite = select_registry_item(
            result.payload["flatItems"],
            object_type="suite",
            required_module_ids=["hosted-runtime"],
            required_supported_hosts=["macos"],
        )
        managed_closure_module = next(item for item in result.payload["flatItems"] if item["id"] == "module/managed-closure/basic")
        self.assertEqual("run test module --id module/managed-closure/basic", managed_closure_module["canonicalCommand"])
        self.assertEqual(
            {"recommended", "requiredBeforeCompletion"},
            set(managed_closure_module["skillRecommendations"].keys()),
        )
        self.assertEqual(
            ["module/managed-closure/basic"],
            [item["objectId"] for item in managed_closure_module["skillRecommendations"]["requiredBeforeCompletion"]],
        )
        module_item = next(item for item in result.payload["flatItems"] if item["id"] == "module/managed-smoke/basic")
        self.assertEqual("run test module --id module/managed-smoke/basic", module_item["canonicalCommand"])
        self.assertEqual(
            {"recommended", "requiredBeforeCompletion"},
            set(module_item["skillRecommendations"].keys()),
        )
        self.assertEqual(
            ["module/managed-smoke/basic"],
            [item["objectId"] for item in module_item["skillRecommendations"]["requiredBeforeCompletion"]],
        )
        self.assertIn(
            managed_runtime_suite["id"],
            [item["objectId"] for item in module_item["skillRecommendations"]["recommended"]],
        )
        suite_item = next(item for item in result.payload["flatItems"] if item["id"] == managed_runtime_suite["id"])
        self.assertNotIn(
            reflection_suite["id"],
            [item["objectId"] for item in suite_item["skillRecommendations"]["recommended"]],
        )
        self.assertEqual(
            ["module/reflection/basic"],
            [item["objectId"] for item in reflection_suite["skillRecommendations"]["requiredBeforeCompletion"]],
        )
        analysis_suite = next(item for item in result.payload["flatItems"] if item["id"] == "contract/analysis-schema")
        self.assertEqual(
            ["module/analysis/basic"],
            [item["objectId"] for item in analysis_suite["skillRecommendations"]["requiredBeforeCompletion"]],
        )
        managed_closure_suite = next(item for item in result.payload["flatItems"] if item["id"] == "contract/managed-closure-bundle")
        self.assertEqual(
            ["module/managed-closure/basic"],
            [item["objectId"] for item in managed_closure_suite["skillRecommendations"]["requiredBeforeCompletion"]],
        )
        self.assertEqual(
            ["module/interop/basic"],
            [item["objectId"] for item in interop_suite["skillRecommendations"]["requiredBeforeCompletion"]],
        )
        native_abi_suite = next(item for item in result.payload["flatItems"] if item["id"] == "contract/native-abi")
        self.assertEqual(
            ["module/interop/basic"],
            [item["objectId"] for item in native_abi_suite["skillRecommendations"]["requiredBeforeCompletion"]],
        )
        native_bridge_suite = next(item for item in result.payload["flatItems"] if item["id"] == "contract/native-bridge")
        self.assertEqual(
            ["module/interop/basic"],
            [item["objectId"] for item in native_bridge_suite["skillRecommendations"]["requiredBeforeCompletion"]],
        )
        trace_schema_suite = next(item for item in result.payload["flatItems"] if item["id"] == "contract/trace-schema")
        self.assertEqual(
            ["module/trace-export/macos"],
            [item["objectId"] for item in trace_schema_suite["skillRecommendations"]["requiredBeforeCompletion"]],
        )
        self.assertEqual(
            ["module/hosted-runtime/basic"],
            [item["objectId"] for item in hosted_runtime_suite["skillRecommendations"]["requiredBeforeCompletion"]],
        )
        trace_suite = next(item for item in result.payload["flatItems"] if item["id"] == "contract/trace-compare-macos")
        self.assertEqual(
            ["module/trace-export/macos"],
            [item["objectId"] for item in trace_suite["skillRecommendations"]["requiredBeforeCompletion"]],
        )
        trace_system = next(item for item in result.payload["flatItems"] if item["id"] == "system/trace-export-macos-smoke")
        self.assertEqual(
            "run test system --id system/trace-export-macos-smoke",
            trace_system["canonicalCommand"],
        )
        roadmap_system = next(item for item in result.payload["flatItems"] if item["id"] == "system/runtime-baseline-macos")
        self.assertEqual(
            "run test system --id system/runtime-baseline-macos",
            roadmap_system["canonicalCommand"],
        )
        linux_gate_suite = next(item for item in result.payload["flatItems"] if item["id"] == "gate/linux-x64-packaging")
        self.assertEqual(
            "run test suite --id gate/linux-x64-packaging",
            linux_gate_suite["canonicalCommand"],
        )
        ios_gate_suite = next(item for item in result.payload["flatItems"] if item["id"] == "gate/ios-arm64-packaging")
        self.assertEqual(
            "run test suite --id gate/ios-arm64-packaging",
            ios_gate_suite["canonicalCommand"],
        )
        macos_reference_gate_suite = next(item for item in result.payload["flatItems"] if item["id"] == "gate/macos-reference-desktop")
        self.assertEqual(
            "run test suite --id gate/macos-reference-desktop",
            macos_reference_gate_suite["canonicalCommand"],
        )
        ios_gate_system = next(item for item in result.payload["flatItems"] if item["id"] == "system/ios-packaging-gate")
        self.assertEqual(
            "run test system --id system/ios-packaging-gate",
            ios_gate_system["canonicalCommand"],
        )
        macos_reference_gate_system = next(item for item in result.payload["flatItems"] if item["id"] == "system/macos-reference-gate")
        self.assertEqual(
            "run test system --id system/macos-reference-gate",
            macos_reference_gate_system["canonicalCommand"],
        )
        self.assertEqual(
            {"recommended", "requiredBeforeCompletion"},
            set(macos_reference_gate_system["skillRecommendations"].keys()),
        )
        linux_gate_system = next(item for item in result.payload["flatItems"] if item["id"] == "system/linux-packaging-gate")
        self.assertEqual(
            "run test system --id system/linux-packaging-gate",
            linux_gate_system["canonicalCommand"],
        )
        trace_module = next(item for item in result.payload["flatItems"] if item["id"] == "module/trace-export/macos")
        self.assertEqual(
            {"recommended", "requiredBeforeCompletion"},
            set(trace_module["skillRecommendations"].keys()),
        )

    def test_registry_list_returns_windows_android_gate_objects(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_registry_list_windows_android_gate")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_registry_list_windows_android_gate")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        result = test_module.handle(
            {"id": "test-registry-list", "handler": "test.dispatch"},
            REPO_ROOT,
            "windows",
            "test registry list",
            manifest,
            {},
        )

        self.assertEqual("ok", result.status)
        flat_ids = {item["id"] for item in result.payload["flatItems"]}
        self.assertIn("gate/android-arm64-smoke", flat_ids)
        self.assertIn("gate/windows-reference-desktop", flat_ids)
        self.assertIn("system/android-startup-gate", flat_ids)
        self.assertIn("system/windows-reference-gate", flat_ids)
        android_gate_suite = next(item for item in result.payload["flatItems"] if item["id"] == "gate/android-arm64-smoke")
        self.assertEqual(
            "run test suite --id gate/android-arm64-smoke",
            android_gate_suite["canonicalCommand"],
        )
        windows_reference_gate_suite = next(item for item in result.payload["flatItems"] if item["id"] == "gate/windows-reference-desktop")
        self.assertEqual(
            "run test suite --id gate/windows-reference-desktop",
            windows_reference_gate_suite["canonicalCommand"],
        )
        android_gate_system = next(item for item in result.payload["flatItems"] if item["id"] == "system/android-startup-gate")
        self.assertEqual(
            "run test system --id system/android-startup-gate",
            android_gate_system["canonicalCommand"],
        )
        windows_reference_gate_system = next(item for item in result.payload["flatItems"] if item["id"] == "system/windows-reference-gate")
        self.assertEqual(
            "run test system --id system/windows-reference-gate",
            windows_reference_gate_system["canonicalCommand"],
        )
        self.assertEqual(
            {"recommended", "requiredBeforeCompletion"},
            set(windows_reference_gate_system["skillRecommendations"].keys()),
        )
        subject_item = select_registry_item(
            result.payload["flatItems"],
            object_type="subject",
            required_supported_hosts=["windows"],
        )
        self.assertEqual("subject", subject_item["type"])
        self.assertEqual(
            f"run test subject --id {subject_item['id']}",
            subject_item["canonicalCommand"],
        )
