from __future__ import annotations

import importlib.util
import io
import sys
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
RUN_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "run.py"
TEST_COMMAND_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "test.py"
MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "core" / "manifest.py"
RESULT_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "core" / "result.py"
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


class StageExecutionTests(unittest.TestCase):
    def test_public_test_handle_surfaces_session_stage_results(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_for_stage")
        manifest_module = load_module(MANIFEST_MODULE_PATH, "chaos_run_manifest_for_stage")
        result_module = load_module(RESULT_MODULE_PATH, "chaos_run_result_for_stage")
        manifest = manifest_module.load_run_manifest(REPO_ROOT, RUN_MANIFEST_PATH)
        legacy_result = result_module.CommandResult.success(
            command="test smoke HelloWorld --stage run",
            host_platform="macos",
            target="HelloWorld",
            payload={"artifacts": ["artifacts/smoke/bin/HelloWorld/Release/net8.0/HelloWorld.dll"]},
            text="managed smoke ok\n",
        )

        with patch.object(test_module, "_execute_legacy_command", return_value=legacy_result):
            result = test_module.handle(
                {"id": "test-family-suite", "handler": "test.dispatch"},
                REPO_ROOT,
                "macos",
                "test smoke HelloWorld --stage run",
                manifest,
                {"family": "smoke", "suite": "HelloWorld", "stage": "run"},
            )

        self.assertEqual("ok", result.status)
        self.assertTrue(result.payload["suiteResults"][0]["stageResults"]["build"]["implicit"])

    def test_explicit_incompatible_suite_fails_with_contract_exit_code(self) -> None:
        run_module = load_module(RUN_MODULE_PATH, "chaos_run_main_for_stage_exit_code")
        stdout = io.StringIO()

        with patch.object(run_module.manifest_module, "is_interactive_session", return_value=False):
            with patch.object(run_module.runtime_module, "detect_host_platform", return_value="macos-arm64"):
                with patch.object(run_module.sys, "stdout", stdout):
                    exit_code = run_module.main(["test", "workflow", "roadmap-0-windows"])

        self.assertEqual(2, exit_code)
        self.assertIn("not supported on host", stdout.getvalue())

    def test_strict_invalid_suite_returns_contract_exit_code(self) -> None:
        run_module = load_module(RUN_MODULE_PATH, "chaos_run_main_for_strict_exit_code")
        stdout = io.StringIO()

        with patch.object(run_module.manifest_module, "is_interactive_session", return_value=False):
            with patch.object(run_module.runtime_module, "detect_host_platform", return_value="macos-arm64"):
                with patch.object(run_module.test_commands.catalog_module, "scan_catalog", side_effect=ValueError("catalog scan found invalid suites")):
                    with patch.object(run_module.sys, "stdout", stdout):
                        exit_code = run_module.main(["test", "all", "--strict"])

        self.assertEqual(2, exit_code)
        self.assertIn("invalid", stdout.getvalue())


if __name__ == "__main__":
    unittest.main()
