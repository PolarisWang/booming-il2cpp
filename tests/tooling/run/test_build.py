from __future__ import annotations

import importlib.util
import subprocess
import sys
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
BUILD_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "build.py"


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


class BuildCommandTests(unittest.TestCase):
    def test_native_contract_build_uses_run_scoped_binary_dir_before_configure(self) -> None:
        build_module = load_module(BUILD_MODULE_PATH, "chaos_run_build")
        command = {
            "kind": "native-contract",
            "target": "abi",
            "source_dir": "tests/contracts/native/abi",
            "binary_dir": "artifacts/run/native-contract-abi",
        }
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        allocated_dir = REPO_ROOT / "artifacts" / "run" / "native-contract-abi-test-run"

        with patch.object(build_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
            with patch.object(build_module, "allocate_run_scoped_binary_dir", return_value=allocated_dir) as allocate_binary_dir:
                with patch.object(build_module, "run_process", return_value=completed) as run_process:
                    with patch.object(build_module, "_reset_binary_dir") as reset_binary_dir:
                        result = build_module.handle(command, REPO_ROOT, "windows", "build native-contract abi")

        self.assertEqual("ok", result.status)
        allocate_binary_dir.assert_called_once_with(REPO_ROOT / "artifacts" / "run" / "native-contract-abi")
        reset_binary_dir.assert_not_called()
        self.assertEqual(
            [
                ["cmake", "-S", str(REPO_ROOT / "tests/contracts/native/abi"), "-B", str(allocated_dir)],
                ["cmake", "--build", str(allocated_dir)],
            ],
            [call.args[0] for call in run_process.call_args_list],
        )
        self.assertEqual("Run completed: build native-contract abi\n", result.text)
        self.assertIn("consoleText", result.payload)

    def test_preset_build_uses_run_scoped_binary_dir_before_configure(self) -> None:
        build_module = load_module(BUILD_MODULE_PATH, "chaos_run_build_preset_scoped_binary_dir")
        command = {
            "kind": "preset",
            "target": "windows-x64-reference",
            "preset": "windows-x64-reference",
            "binary_dir": "artifacts/presets/windows-x64-reference",
        }
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        allocated_dir = REPO_ROOT / "artifacts" / "presets" / "windows-x64-reference-test-run"

        with patch.object(build_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
            with patch.object(build_module, "allocate_run_scoped_binary_dir", return_value=allocated_dir) as allocate_binary_dir:
                with patch.object(build_module, "run_process", return_value=completed) as run_process:
                    with patch.object(build_module, "_reset_binary_dir") as reset_binary_dir:
                        result = build_module.handle(command, REPO_ROOT, "windows", "build windows-x64-reference")

        self.assertEqual("ok", result.status)
        allocate_binary_dir.assert_called_once_with(REPO_ROOT / "artifacts" / "presets" / "windows-x64-reference")
        reset_binary_dir.assert_not_called()
        self.assertEqual(
            [
                ["cmake", "--preset", "windows-x64-reference", "-B", str(allocated_dir)],
                ["cmake", "--build", str(allocated_dir)],
            ],
            [call.args[0] for call in run_process.call_args_list],
        )
        self.assertEqual("Run completed: build windows-x64-reference\n", result.text)

    def test_windows_reference_preset_refreshes_subject_exec_generated_root_when_missing(self) -> None:
        build_module = load_module(BUILD_MODULE_PATH, "chaos_run_build_preset_refreshes_subject_exec")
        command = {
            "kind": "preset",
            "target": "windows-x64-reference",
            "preset": "windows-x64-reference",
            "binary_dir": "artifacts/presets/windows-x64-reference",
        }
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")
        allocated_dir = REPO_ROOT / "artifacts" / "presets" / "windows-x64-reference-test-run"
        missing_generated = (
            REPO_ROOT
            / "artifacts"
            / "subjects"
            / "SolutionCorePack"
            / "runs"
            / "subject-exec"
            / "analysis"
            / "generated"
            / "generated"
            / "missing.cpp"
        )

        with patch.object(build_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
            with patch.object(build_module, "allocate_run_scoped_binary_dir", return_value=allocated_dir):
                with patch.object(build_module, "run_process", return_value=completed):
                    with patch.object(
                        build_module.workspace_module,
                        "_subject_generated_source_path",
                        return_value=missing_generated,
                    ):
                        with patch.object(build_module.workspace_module, "refresh_subject_generated_root") as refresh_mock:
                            result = build_module.handle(command, REPO_ROOT, "windows", "build windows-x64-reference")

        self.assertEqual("ok", result.status)
        refresh_mock.assert_called_once_with(REPO_ROOT, "SolutionCorePack", "windows-native-check", "CHECK")


if __name__ == "__main__":
    unittest.main()
