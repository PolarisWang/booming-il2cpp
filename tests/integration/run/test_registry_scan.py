from __future__ import annotations

import importlib.util
import json
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
REGISTRY_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "registry.py"
PUBLIC_SPECS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "public_specs.py"


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class RegistryScanTests(unittest.TestCase):
    def test_registry_scan_collects_suites_modules_systems_and_pipelines(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "booming_run_registry")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "booming_run_public_specs")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="macos",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        object_ids = {item["id"] for item in index.flat_items}
        self.assertIn("contract/analysis-schema", object_ids)
        self.assertIn("contract/trace-schema", object_ids)
        self.assertIn("contract/native-abi", object_ids)
        self.assertIn("contract/native-bridge", object_ids)
        self.assertIn("module/analysis/basic", object_ids)
        self.assertIn("smoke/HelloWorld", object_ids)
        self.assertIn("module/managed-smoke/basic", object_ids)
        self.assertIn("module/reflection/basic", object_ids)
        self.assertIn("module/interop/basic", object_ids)
        self.assertIn("module/hosted-runtime/basic", object_ids)
        self.assertIn("module/trace-export/macos", object_ids)
        self.assertIn("gate/ios-arm64-packaging", object_ids)
        self.assertIn("gate/macos-reference-desktop", object_ids)
        self.assertIn("system/hosted-runtime-smoke", object_ids)
        self.assertIn("system/roadmap-0-ios-packaging-gate", object_ids)
        self.assertIn("gate/linux-x64-packaging", object_ids)
        self.assertIn("system/roadmap-0-linux-packaging-gate", object_ids)
        self.assertIn("system/roadmap-0-macos-reference-gate", object_ids)
        self.assertIn("system/roadmap-0-macos", object_ids)
        self.assertIn("system/trace-export-macos-smoke", object_ids)
        self.assertIn("pipeline/completion-runtime-core", object_ids)
        self.assertIn("pipeline/completion-runtime-trace-macos", object_ids)
        self.assertIn("pipeline/trace-export-macos-runtime", object_ids)
        self.assertNotIn("module/trace-export/windows", object_ids)
        self.assertNotIn("system/roadmap-0-windows", object_ids)
        self.assertNotIn("system/trace-export-windows-smoke", object_ids)
        self.assertNotIn("pipeline/completion-runtime-trace-windows", object_ids)
        self.assertNotIn("pipeline/trace-export-windows-runtime", object_ids)
        self.assertNotIn("gate/android-arm64-smoke", object_ids)
        self.assertNotIn("gate/windows-reference-desktop", object_ids)
        self.assertNotIn("system/roadmap-0-android-startup-gate", object_ids)
        self.assertNotIn("system/roadmap-0-windows-reference-gate", object_ids)
        self.assertNotIn("subject/HelloWorldObject", object_ids)

    def test_registry_scan_collects_windows_android_gate_objects(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "booming_run_registry_windows_android_gate")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "booming_run_public_specs_windows_android_gate")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="windows",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        object_ids = {item["id"] for item in index.flat_items}
        self.assertIn("gate/android-arm64-smoke", object_ids)
        self.assertIn("gate/windows-reference-desktop", object_ids)
        self.assertIn("system/roadmap-0-android-startup-gate", object_ids)
        self.assertIn("system/roadmap-0-windows-reference-gate", object_ids)
        self.assertIn("subject/HelloWorld", object_ids)
        self.assertIn("subject/HelloWorldObject", object_ids)
        self.assertIn("subject/GenericEcho", object_ids)
        self.assertIn("subject/ReflectionLite", object_ids)
        self.assertIn("subject/PInvokeLite", object_ids)
        self.assertIn("subject/HostEmbeddingLite", object_ids)
        self.assertNotIn("system/roadmap-0-linux-packaging-gate-macos-only", object_ids)
        hello_world_item = next(item for item in index.flat_items if item["id"] == "subject/HelloWorld")
        self.assertEqual("canonical", hello_world_item["category"])
        self.assertEqual("correctness.dev", hello_world_item["defaultGoalId"])
        self.assertEqual("windows-managed-output", hello_world_item["defaultMatrixId"])
        subject_item = next(item for item in index.flat_items if item["id"] == "subject/HelloWorldObject")
        self.assertEqual("subject", subject_item["type"])
        self.assertEqual(
            "run test subject --id subject/HelloWorldObject",
            subject_item["canonicalCommand"],
        )
        self.assertEqual(["windows"], subject_item["supportedHosts"])
        generic_echo_item = next(item for item in index.flat_items if item["id"] == "subject/GenericEcho")
        self.assertEqual("benchmark", generic_echo_item["category"])
        self.assertEqual("perf.dev", generic_echo_item["defaultGoalId"])
        self.assertEqual("windows-perf-dev", generic_echo_item["defaultMatrixId"])
        self.assertEqual(
            "run test subject --id subject/GenericEcho",
            generic_echo_item["canonicalCommand"],
        )
        self.assertEqual(["perf.dev", "perf.release"], generic_echo_item["goalIds"])

    def test_pipeline_execution_plan_deduplicates_suite_runs(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "booming_run_registry_for_pipeline_plan")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "booming_run_public_specs_for_pipeline_plan")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="macos",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        plan = registry_module.expand_execution_plan(index, "pipeline/completion-runtime-core")

        self.assertEqual(
            [
                "smoke/HelloWorld",
                "smoke/GenericEcho",
                "smoke/ReflectionLite",
                "smoke/PInvokeLite",
                "smoke/HostEmbeddingLite",
                "contract/analysis-schema",
                "contract/native-abi",
                "contract/native-bridge",
            ],
            [item["id"] for item in plan],
        )

    def test_trace_export_pipeline_execution_plan_is_host_specific(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "booming_run_registry_for_trace_export_plan")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "booming_run_public_specs_for_trace_export_plan")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="macos",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        plan = registry_module.expand_execution_plan(index, "pipeline/trace-export-macos-runtime")

        self.assertEqual(
            [
                "smoke/HostEmbeddingLite",
                "contract/trace-schema",
                "contract/trace-compare-macos",
            ],
            [item["id"] for item in plan],
        )

    def test_completion_trace_pipeline_extends_completion_core_on_supported_hosts(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "booming_run_registry_for_completion_trace_plan")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "booming_run_public_specs_for_completion_trace_plan")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="macos",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        plan = registry_module.expand_execution_plan(index, "pipeline/completion-runtime-trace-macos")

        self.assertEqual(
            [
                "smoke/HelloWorld",
                "smoke/GenericEcho",
                "smoke/ReflectionLite",
                "smoke/PInvokeLite",
                "smoke/HostEmbeddingLite",
                "contract/analysis-schema",
                "contract/native-abi",
                "contract/native-bridge",
                "contract/trace-schema",
                "contract/trace-compare-macos",
                "gate/macos-reference-desktop",
            ],
            [item["id"] for item in plan],
        )

    def test_refresh_writes_current_and_history_snapshots(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "booming_run_registry_for_snapshot")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "booming_run_public_specs_for_snapshot")

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


if __name__ == "__main__":
    unittest.main()
