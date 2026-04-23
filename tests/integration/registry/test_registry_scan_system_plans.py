from tests.integration.registry.registry_scan_test_support import *


class TestRegistryScanSystemPlans(RegistryScanTestSupport):
    def test_runtime_baseline_system_execution_plan_deduplicates_suite_runs(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_for_runtime_baseline_system_plan")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs_for_runtime_baseline_system_plan")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="macos",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        plan = registry_module.expand_execution_plan(index, "system/runtime-baseline-macos")

        self.assertEqual(
            [
                "contract/analysis-schema",
                "smoke/managed-entry-basic",
                "smoke/managed-generics-basic",
                "smoke/reflection-basic",
                "contract/native-abi",
                "contract/native-bridge",
                "smoke/native-interop-basic",
                "smoke/host-embedding-basic",
                "contract/trace-schema",
                "contract/trace-compare-macos",
                "gate/macos-reference-desktop",
                "gate/ios-arm64-packaging",
                "gate/linux-x64-packaging",
            ],
            [item["id"] for item in plan],
        )

    def test_trace_export_system_execution_plan_is_host_specific(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_for_trace_export_system_plan")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs_for_trace_export_system_plan")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="macos",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        plan = registry_module.expand_execution_plan(index, "system/trace-export-macos-smoke")

        self.assertEqual(
            [
                "smoke/host-embedding-basic",
                "contract/trace-schema",
                "contract/trace-compare-macos",
            ],
            [item["id"] for item in plan],
        )

    def test_refresh_writes_current_and_history_snapshots_without_pipeline_objects(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_for_snapshot")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs_for_snapshot")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="macos",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )
        snapshot = registry_module.write_registry_snapshot(
            REPO_ROOT,
            index,
            stamp="20260404-210000",
        )

        current_path = REPO_ROOT / "artifacts" / "tests" / "registry" / "current" / "index.json"
        history_path = REPO_ROOT / "artifacts" / "tests" / "registry" / "history" / "20260404-210000" / "index.json"

        self.assertEqual(current_path, snapshot["currentPath"])
        self.assertEqual(history_path, snapshot["historyPath"])
        self.assertTrue(current_path.is_file())
        self.assertTrue(history_path.is_file())

        payload = json.loads(current_path.read_text(encoding="utf-8"))
        self.assertIn("flatItems", payload)
        self.assertTrue(any(item["id"] == "module/analysis/basic" for item in payload["flatItems"]))
        self.assertTrue(any(item["id"] == "module/managed-smoke/basic" for item in payload["flatItems"]))
        self.assertTrue(any(item["id"] == "module/reflection/basic" for item in payload["flatItems"]))
        self.assertTrue(any(item["id"] == "module/interop/basic" for item in payload["flatItems"]))
        self.assertTrue(any(item["id"] == "module/hosted-runtime/basic" for item in payload["flatItems"]))
        self.assertNotIn("pipelines", payload)
        self.assertTrue(all(not str(item["id"]).startswith("pipeline/") for item in payload["flatItems"]))
