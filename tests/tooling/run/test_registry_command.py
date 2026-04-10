from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path
from unittest.mock import patch

from tests.support import load_public_specs_module, select_public_suite_spec, select_registry_item


REPO_ROOT = Path(__file__).resolve().parents[3]
TEST_COMMAND_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "test.py"
MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "core" / "manifest.py"
SESSION_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "session.py"
RUN_MANIFEST_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run_manifest.json"


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


class RegistryCommandTests(unittest.TestCase):
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
        self.assertIn("pipeline/completion-managed-closure", flat_ids)
        self.assertIn("pipeline/completion-runtime-core", flat_ids)
        self.assertIn("pipeline/completion-runtime-trace-macos", flat_ids)
        self.assertIn("pipeline/trace-export-macos-runtime", flat_ids)
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
            ["module/managed-closure/basic"],
            [item["objectId"] for item in managed_closure_module["skillRecommendations"]["requiredBeforeCompletion"]],
        )
        self.assertIn(
            "pipeline/completion-managed-closure",
            [item["objectId"] for item in managed_closure_module["skillRecommendations"]["requiredForPipelineRelease"]],
        )
        module_item = next(item for item in result.payload["flatItems"] if item["id"] == "module/managed-smoke/basic")
        self.assertEqual("run test module --id module/managed-smoke/basic", module_item["canonicalCommand"])
        self.assertEqual(
            ["module/managed-smoke/basic"],
            [item["objectId"] for item in module_item["skillRecommendations"]["requiredBeforeCompletion"]],
        )
        self.assertIn(
            "pipeline/completion-runtime-core",
            [item["objectId"] for item in module_item["skillRecommendations"]["requiredForPipelineRelease"]],
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
        self.assertIn(
            "pipeline/completion-runtime-trace-macos",
            [item["objectId"] for item in macos_reference_gate_system["skillRecommendations"]["requiredForPipelineRelease"]],
        )
        linux_gate_system = next(item for item in result.payload["flatItems"] if item["id"] == "system/linux-packaging-gate")
        self.assertEqual(
            "run test system --id system/linux-packaging-gate",
            linux_gate_system["canonicalCommand"],
        )
        trace_module = next(item for item in result.payload["flatItems"] if item["id"] == "module/trace-export/macos")
        self.assertIn(
            "pipeline/completion-runtime-trace-macos",
            [item["objectId"] for item in trace_module["skillRecommendations"]["requiredForPipelineRelease"]],
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
        self.assertIn(
            "pipeline/completion-runtime-trace-windows",
            [item["objectId"] for item in windows_reference_gate_system["skillRecommendations"]["requiredForPipelineRelease"]],
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

    def test_module_dispatch_expands_registered_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_module_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_module_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_module_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-module", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test module --id module/analysis/basic",
                manifest,
                {"id": "module/analysis/basic"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("module/analysis/basic", result.target)
        self.assertEqual("module/analysis/basic", result.payload["selectedObject"]["id"])
        self.assertEqual(
            ["contract/analysis-schema"],
            [item["id"] for item in result.payload["items"]],
        )

    def test_managed_closure_module_dispatch_expands_contract_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_managed_closure_module_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_managed_closure_module_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_managed_closure_module_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-module", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test module --id module/managed-closure/basic",
                manifest,
                {"id": "module/managed-closure/basic"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("module/managed-closure/basic", result.target)
        self.assertEqual("module/managed-closure/basic", result.payload["selectedObject"]["id"])
        self.assertEqual(
            ["contract/managed-closure-bundle"],
            [item["id"] for item in result.payload["items"]],
        )

    def test_interop_module_dispatch_expands_contract_and_runtime_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_interop_module_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_interop_module_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_interop_module_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-module", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test module --id module/interop/basic",
                manifest,
                {"id": "module/interop/basic"},
            )

        self.assertEqual("ok", result.status)
        runtime_suite = select_public_suite_spec(
            "chaos_registry_interop_runtime_suite",
            host_platform="macos",
            family="smoke",
            required_module_ids=["interop"],
        )
        self.assertEqual(
            ["contract/native-abi", "contract/native-bridge", str(runtime_suite["id"])],
            [item["id"] for item in result.payload["items"]],
        )

    def test_trace_export_module_dispatch_includes_trace_schema_contract(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_trace_export_module_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_trace_export_module_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_trace_export_module_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-module", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test module --id module/trace-export/macos",
                manifest,
                {"id": "module/trace-export/macos"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual(
            ["contract/trace-schema", "contract/trace-compare-macos"],
            [item["id"] for item in result.payload["items"]],
        )

    def test_pipeline_dispatch_returns_phase_results(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_pipeline_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_pipeline_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_pipeline_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-pipeline", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test pipeline --id pipeline/completion-runtime-core",
                manifest,
                {"id": "pipeline/completion-runtime-core"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("pipeline/completion-runtime-core", result.payload["selectedObject"]["id"])
        self.assertEqual(
            ["code", "module", "system"],
            [item["phaseId"] for item in result.payload["phaseResults"]],
        )
        module_phase = next(item for item in result.payload["phaseResults"] if item["phaseId"] == "module")
        self.assertEqual("ok", module_phase["status"])
        self.assertIn(
            "module/reflection/basic",
            [member["objectId"] for member in module_phase["memberResults"]],
        )
        system_phase = next(item for item in result.payload["phaseResults"] if item["phaseId"] == "system")
        self.assertEqual(
            ["system/hosted-runtime-smoke"],
            [member["objectId"] for member in system_phase["memberResults"]],
        )

    def test_managed_closure_pipeline_dispatch_returns_code_and_module_phases(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_managed_closure_pipeline_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_managed_closure_pipeline_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_managed_closure_pipeline_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-pipeline", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test pipeline --id pipeline/completion-managed-closure",
                manifest,
                {"id": "pipeline/completion-managed-closure"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("pipeline/completion-managed-closure", result.payload["selectedObject"]["id"])
        self.assertEqual(
            ["code", "module"],
            [item["phaseId"] for item in result.payload["phaseResults"]],
        )
        code_phase = next(item for item in result.payload["phaseResults"] if item["phaseId"] == "code")
        self.assertEqual(
            ["contract/managed-closure-bundle"],
            [member["objectId"] for member in code_phase["memberResults"]],
        )
        module_phase = next(item for item in result.payload["phaseResults"] if item["phaseId"] == "module")
        self.assertEqual(
            ["module/managed-closure/basic"],
            [member["objectId"] for member in module_phase["memberResults"]],
        )

    def test_completion_trace_pipeline_dispatch_includes_reference_gate_in_system_phase(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_completion_trace_pipeline_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_completion_trace_pipeline_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_completion_trace_pipeline_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-pipeline", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test pipeline --id pipeline/completion-runtime-trace-macos",
                manifest,
                {"id": "pipeline/completion-runtime-trace-macos"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("pipeline/completion-runtime-trace-macos", result.payload["selectedObject"]["id"])
        system_phase = next(item for item in result.payload["phaseResults"] if item["phaseId"] == "system")
        self.assertEqual(
            [
                "system/hosted-runtime-smoke",
                "system/trace-export-macos-smoke",
                "system/macos-reference-gate",
            ],
            [member["objectId"] for member in system_phase["memberResults"]],
        )

    def test_system_dispatch_expands_registered_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_system_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_system_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_system_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-system", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test system --id system/runtime-baseline-macos",
                manifest,
                {"id": "system/runtime-baseline-macos"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("system/runtime-baseline-macos", result.target)
        self.assertEqual("system/runtime-baseline-macos", result.payload["selectedObject"]["id"])
        self.assertEqual(
            "run test system --id system/runtime-baseline-macos",
            result.payload["selectedObject"]["canonicalCommand"],
        )
        public_specs_module = load_public_specs_module("chaos_registry_system_dispatch_public_specs")
        expected_smoke_suite_ids = [
            str(spec["id"])
            for spec in list(public_specs_module.PUBLIC_TEST_SPECS)
            if str(spec.get("family") or "") == "smoke" and "macos" in list(spec.get("supported_hosts") or [])
        ]
        self.assertEqual(
            [
                *expected_smoke_suite_ids,
                "contract/analysis-schema",
                "contract/native-abi",
                "contract/native-bridge",
                "contract/trace-schema",
                "contract/trace-compare-macos",
                "gate/macos-reference-desktop",
                "gate/ios-arm64-packaging",
                "gate/linux-x64-packaging",
            ],
            [item["id"] for item in result.payload["items"]],
        )

    def test_legacy_runtime_baseline_system_id_maps_to_canonical_object(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_system_dispatch_legacy_alias")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_system_dispatch_legacy_alias")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_system_dispatch_legacy_alias")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-system", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test system --id system/roadmap-0-macos",
                manifest,
                {"id": "system/roadmap-0-macos"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("system/runtime-baseline-macos", result.target)
        self.assertEqual("system/runtime-baseline-macos", result.payload["selectedObject"]["id"])
        self.assertEqual(
            "run test system --id system/runtime-baseline-macos",
            result.payload["selectedObject"]["canonicalCommand"],
        )

    def test_linux_gate_system_dispatch_expands_registered_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_linux_gate_system_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_linux_gate_system_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_linux_gate_system_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-system", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test system --id system/linux-packaging-gate",
                manifest,
                {"id": "system/linux-packaging-gate"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("system/linux-packaging-gate", result.target)
        self.assertEqual("system/linux-packaging-gate", result.payload["selectedObject"]["id"])
        self.assertEqual(
            "run test system --id system/linux-packaging-gate",
            result.payload["selectedObject"]["canonicalCommand"],
        )
        self.assertEqual(
            ["gate/linux-x64-packaging"],
            [item["id"] for item in result.payload["items"]],
        )

    def test_legacy_linux_gate_system_id_maps_to_canonical_object(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_linux_gate_system_dispatch_legacy_alias")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_linux_gate_system_dispatch_legacy_alias")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_linux_gate_system_dispatch_legacy_alias")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-system", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test system --id system/roadmap-0-linux-packaging-gate",
                manifest,
                {"id": "system/roadmap-0-linux-packaging-gate"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("system/linux-packaging-gate", result.target)
        self.assertEqual("system/linux-packaging-gate", result.payload["selectedObject"]["id"])
        self.assertEqual(
            "run test system --id system/linux-packaging-gate",
            result.payload["selectedObject"]["canonicalCommand"],
        )

    def test_ios_gate_system_dispatch_expands_registered_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_ios_gate_system_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_ios_gate_system_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_ios_gate_system_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-system", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test system --id system/ios-packaging-gate",
                manifest,
                {"id": "system/ios-packaging-gate"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("system/ios-packaging-gate", result.target)
        self.assertEqual("system/ios-packaging-gate", result.payload["selectedObject"]["id"])
        self.assertEqual(
            "run test system --id system/ios-packaging-gate",
            result.payload["selectedObject"]["canonicalCommand"],
        )
        self.assertEqual(
            ["gate/ios-arm64-packaging"],
            [item["id"] for item in result.payload["items"]],
        )

    def test_android_gate_system_dispatch_expands_registered_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_android_gate_system_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_android_gate_system_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_android_gate_system_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-system", "handler": "test.dispatch"},
                REPO_ROOT,
                "windows",
                "test system --id system/android-startup-gate",
                manifest,
                {"id": "system/android-startup-gate"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("system/android-startup-gate", result.target)
        self.assertEqual("system/android-startup-gate", result.payload["selectedObject"]["id"])
        self.assertEqual(
            "run test system --id system/android-startup-gate",
            result.payload["selectedObject"]["canonicalCommand"],
        )
        self.assertEqual(
            ["gate/android-arm64-smoke"],
            [item["id"] for item in result.payload["items"]],
        )

    def test_windows_reference_gate_system_dispatch_expands_registered_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_windows_reference_gate_system_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_windows_reference_gate_system_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_windows_reference_gate_system_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-system", "handler": "test.dispatch"},
                REPO_ROOT,
                "windows",
                "test system --id system/windows-reference-gate",
                manifest,
                {"id": "system/windows-reference-gate"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("system/windows-reference-gate", result.target)
        self.assertEqual("system/windows-reference-gate", result.payload["selectedObject"]["id"])
        self.assertEqual(
            "run test system --id system/windows-reference-gate",
            result.payload["selectedObject"]["canonicalCommand"],
        )
        self.assertEqual(
            ["gate/windows-reference-desktop"],
            [item["id"] for item in result.payload["items"]],
        )

    def test_macos_reference_gate_system_dispatch_expands_registered_plan(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_macos_reference_gate_system_dispatch")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_macos_reference_gate_system_dispatch")
        session_module = load_module(SESSION_MODULE_PATH, "chaos_run_session_macos_reference_gate_system_dispatch")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        def fake_session(
            family: str,
            suite: str,
            stage: str,
            repo_root: Path,
            host_platform: str,
            command_text: str,
            manifest_payload: dict,
        ):
            del repo_root
            del manifest_payload
            request = session_module.TestRequest(
                family=family,
                suite=suite,
                stage=stage,
                command_text=command_text,
            )
            return session_module.SessionResult(
                request=request,
                host_platform=host_platform,
                status="ok",
                suite_results=[{"suiteId": request.suite_key, "status": "ok", "stageResults": {}}],
                text=f"{request.suite_key} ok\n",
                artifacts=[],
                exit_code=0,
            )

        with patch.object(test_module, "_execute_public_test_session", side_effect=fake_session):
            result = test_module.handle(
                {"id": "test-system", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test system --id system/macos-reference-gate",
                manifest,
                {"id": "system/macos-reference-gate"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("system/macos-reference-gate", result.target)
        self.assertEqual("system/macos-reference-gate", result.payload["selectedObject"]["id"])
        self.assertEqual(
            "run test system --id system/macos-reference-gate",
            result.payload["selectedObject"]["canonicalCommand"],
        )
        self.assertEqual(
            ["gate/macos-reference-desktop"],
            [item["id"] for item in result.payload["items"]],
        )

    def test_missing_registry_object_returns_structured_hint(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_registry_missing")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_registry_missing")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)

        result = test_module.handle(
            {"id": "test-module", "handler": "test.dispatch"},
            REPO_ROOT,
            "macos",
            "test module --id module/missing/basic",
            manifest,
            {"id": "module/missing/basic"},
        )

        self.assertEqual("error", result.status)
        self.assertEqual("test.registry.object_not_found", result.payload["failureCode"])
        self.assertIn("run test registry list --json", result.payload["parserHint"])
        self.assertIn("module/missing/basic", result.payload["humanHint"])


if __name__ == "__main__":
    unittest.main()
