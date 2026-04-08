from __future__ import annotations

import importlib.util
import platform
import shutil
import subprocess
import sys
import tempfile
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


@unittest.skipUnless(platform.system() == "Darwin", "macOS only")
class PlatformGateValidateOnlyTests(unittest.TestCase):
    def test_macos_reference_preset_configures_with_repo_preset(self) -> None:
        shutil.rmtree(REPO_ROOT / "artifacts" / "presets" / "macos-reference", ignore_errors=True)
        completed = subprocess.run(
            ["cmake", "--preset", "macos-reference"],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
            check=False,
        )

        self.assertEqual(0, completed.returncode, msg=completed.stdout + completed.stderr)

    def test_linux_packaging_validate_only_configures_without_cross_compiler(self) -> None:
        with tempfile.TemporaryDirectory() as temp_dir:
            binary_dir = Path(temp_dir) / "linux-packaging-routing"
            completed = subprocess.run(
                [
                    "cmake",
                    "-S",
                    str(REPO_ROOT),
                    "-B",
                    str(binary_dir),
                    "-G",
                    "Xcode",
                    "-DROADMAP0_PRESET_TARGET=linux-x64-packaging",
                    "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON",
                    f"-DCMAKE_TOOLCHAIN_FILE={REPO_ROOT / 'build' / 'toolchains' / 'linux-x64.cmake'}",
                ],
                cwd=REPO_ROOT,
                capture_output=True,
                text=True,
                check=False,
            )

        self.assertEqual(0, completed.returncode, msg=completed.stdout + completed.stderr)

    def test_platform_gate_uses_preset_compatible_generator(self) -> None:
        build_module = load_module(BUILD_MODULE_PATH, "chaos_run_build_platform_gate")
        completed = subprocess.CompletedProcess(["cmake"], 0, stdout="", stderr="")

        with patch.object(build_module, "run_process", return_value=completed) as run_process_mock:
            linux_result = build_module.handle(
                {
                    "id": "build-platform-linux-x64-packaging",
                    "kind": "platform-gate",
                    "target": "linux-x64-packaging",
                    "preset_target": "linux-x64-packaging",
                    "toolchain_file": "build/toolchains/linux-x64.cmake",
                    "binary_dir": "artifacts/run/platform/linux-x64-packaging",
                },
                REPO_ROOT,
                "macos",
                "build platform linux-x64-packaging",
            )

        self.assertEqual("ok", linux_result.status)
        configure_args = run_process_mock.call_args_list[0].args[0]
        self.assertIn("Xcode", configure_args)

        with patch.object(build_module, "run_process", return_value=completed) as run_process_mock:
            ios_result = build_module.handle(
                {
                    "id": "build-platform-ios-arm64-packaging",
                    "kind": "platform-gate",
                    "target": "ios-arm64-packaging",
                    "preset_target": "ios-arm64-packaging",
                    "toolchain_file": "build/toolchains/ios-arm64.cmake",
                    "binary_dir": "artifacts/run/platform/ios-arm64-packaging",
                },
                REPO_ROOT,
                "macos",
                "build platform ios-arm64-packaging",
            )

        self.assertEqual("ok", ios_result.status)
        configure_args = run_process_mock.call_args_list[0].args[0]
        self.assertIn("Xcode", configure_args)

    def test_platform_gate_uses_run_scoped_binary_dir(self) -> None:
        build_module = load_module(BUILD_MODULE_PATH, "chaos_run_build_platform_gate_scoped_dir")
        completed = subprocess.CompletedProcess(["cmake"], 0, stdout="", stderr="")
        requested_dir = REPO_ROOT / "artifacts" / "run" / "platform" / "linux-x64-packaging"
        allocated_dir = requested_dir.parent / "linux-x64-packaging-test-run"

        with patch.object(build_module, "allocate_run_scoped_binary_dir", return_value=allocated_dir):
            with patch.object(build_module, "run_process", return_value=completed) as run_process_mock:
                result = build_module.handle(
                    {
                        "id": "build-platform-linux-x64-packaging",
                        "kind": "platform-gate",
                        "target": "linux-x64-packaging",
                        "preset_target": "linux-x64-packaging",
                        "toolchain_file": "build/toolchains/linux-x64.cmake",
                        "binary_dir": "artifacts/run/platform/linux-x64-packaging",
                    },
                    REPO_ROOT,
                    "windows",
                    "build platform linux-x64-packaging",
                )

        self.assertEqual("ok", result.status)
        configure_args = run_process_mock.call_args_list[0].args[0]
        build_args = run_process_mock.call_args_list[1].args[0]
        self.assertIn(str(allocated_dir), configure_args)
        self.assertEqual(["--build", str(allocated_dir)], build_args[1:])
        self.assertEqual([str(allocated_dir)], result.payload["artifacts"])


class PlatformGateCommandTests(unittest.TestCase):
    def test_reference_desktop_gate_runs_preset_and_subject_trace_pipeline(self) -> None:
        build_module = load_module(BUILD_MODULE_PATH, "chaos_run_build_reference_desktop_gate")
        bootstrap = build_module.tooling_module.ToolBootstrapResult(
            ready=True,
            output="",
            errors=[],
        )
        requested_dir = REPO_ROOT / "artifacts" / "presets" / "windows-x64-reference"
        allocated_dir = requested_dir.parent / "windows-x64-reference-test-run"
        run_id = "20260408-windows-0001"
        trace_output = REPO_ROOT / "artifacts" / "subjects" / "FixtureTraceSubject" / "runs" / run_id / "matrices" / "windows-reference-trace" / "runtime" / "trace.runtime.json"
        gate_record = REPO_ROOT / "artifacts" / "verify-roadmap-0" / "windows" / "windows-reference-desktop.gate.json"
        completed = subprocess.CompletedProcess(["cmd"], 0, stdout="ok", stderr="")
        execution_result = {
            "subjectId": "FixtureTraceSubject",
            "matrixId": "windows-reference-trace",
            "goalId": "correctness.platform",
            "status": "ok",
            "stageResults": [],
            "errors": [],
        }

        with patch.object(build_module.tooling_module, "ensure_dotnet_available", return_value=bootstrap):
            with patch.object(build_module, "allocate_run_scoped_binary_dir", return_value=allocated_dir):
                with patch.object(build_module, "run_process", return_value=completed) as run_process_mock:
                    with patch.object(build_module.reporting_module, "build_run_id", return_value=run_id):
                        with patch.object(build_module.subject_executor_module, "execute_subject_matrix", return_value=execution_result) as execute_subject_matrix_mock:
                            with patch.object(
                                build_module.subject_executor_module,
                                "trace_paths_from_execution",
                                return_value=["artifacts/subjects/FixtureTraceSubject/runs/20260408-windows-0001/matrices/windows-reference-trace/runtime/trace.runtime.json"],
                            ) as trace_paths_mock:
                                result = build_module.handle(
                                    {
                                        "id": "build-platform-windows-reference-desktop",
                                        "kind": "reference-desktop-gate",
                                        "target": "windows-reference-desktop",
                                        "preset": "windows-x64-reference",
                                        "binary_dir": "artifacts/presets/windows-x64-reference",
                                        "subject_id": "FixtureTraceSubject",
                                        "goal_id": "correctness.platform",
                                        "matrix_id": "windows-reference-trace",
                                        "gate_record_path": "artifacts/verify-roadmap-0/windows/windows-reference-desktop.gate.json",
                                        "gate_name": "windows-reference-desktop",
                                        "gate_preset": "windows-x64-reference",
                                        "gate_notes": "Windows reference desktop gate passed with warmup trace compare.",
                                    },
                                    REPO_ROOT,
                                    "windows",
                                    "build platform windows-reference-desktop",
                                )

        self.assertEqual("ok", result.status)
        self.assertEqual(
            ["--preset", "windows-x64-reference", "-B", str(allocated_dir)],
            run_process_mock.call_args_list[0].args[0][1:],
        )
        self.assertEqual(
            ["--build", str(allocated_dir)],
            run_process_mock.call_args_list[1].args[0][1:],
        )
        self.assertEqual(2, run_process_mock.call_count)
        execute_subject_matrix_mock.assert_called_once_with(
            REPO_ROOT,
            "FixtureTraceSubject",
            goal_id="correctness.platform",
            matrix_id="windows-reference-trace",
            run_id=run_id,
        )
        trace_paths_mock.assert_called_once_with(REPO_ROOT, execution_result)
        self.assertEqual(
            [str(allocated_dir), str(trace_output), str(gate_record)],
            result.payload["artifacts"],
        )
        self.assertTrue(gate_record.is_file())

    def test_reference_desktop_gate_failure_text_includes_underlying_output(self) -> None:
        build_module = load_module(BUILD_MODULE_PATH, "chaos_run_build_reference_desktop_gate_failure")
        bootstrap = build_module.tooling_module.ToolBootstrapResult(
            ready=True,
            output="",
            errors=[],
        )
        success = subprocess.CompletedProcess(["cmd"], 0, stdout="ok", stderr="")
        execution_result = {
            "subjectId": "FixtureTraceSubject",
            "matrixId": "windows-reference-trace",
            "goalId": "correctness.platform",
            "status": "fail",
            "stageResults": [],
            "errors": ["trace compare mismatch at root.warmup[0]"],
        }

        with patch.object(build_module.tooling_module, "ensure_dotnet_available", return_value=bootstrap):
            with patch.object(build_module, "allocate_run_scoped_binary_dir", return_value=REPO_ROOT / "artifacts" / "presets" / "windows-x64-reference-test-run"):
                with patch.object(build_module, "run_process", side_effect=[success, success]):
                    with patch.object(build_module.reporting_module, "build_run_id", return_value="20260408-windows-0002"):
                        with patch.object(build_module.subject_executor_module, "execute_subject_matrix", return_value=execution_result):
                            result = build_module.handle(
                                {
                                    "id": "build-platform-windows-reference-desktop",
                                    "kind": "reference-desktop-gate",
                                    "target": "windows-reference-desktop",
                                    "preset": "windows-x64-reference",
                                    "binary_dir": "artifacts/presets/windows-x64-reference",
                                    "subject_id": "FixtureTraceSubject",
                                    "goal_id": "correctness.platform",
                                    "matrix_id": "windows-reference-trace",
                                    "gate_record_path": "artifacts/verify-roadmap-0/windows/windows-reference-desktop.gate.json",
                                    "gate_name": "windows-reference-desktop",
                                    "gate_preset": "windows-x64-reference",
                                    "gate_notes": "Windows reference desktop gate passed with warmup trace compare.",
                                },
                                REPO_ROOT,
                                "windows",
                                "build platform windows-reference-desktop",
                            )

        self.assertEqual("error", result.status)
        self.assertIn("trace compare mismatch at root.warmup[0]", result.payload["consoleText"])
        self.assertIn("trace compare mismatch at root.warmup[0]", result.text)

    def test_reference_desktop_gate_refreshes_subject_exec_generated_root_when_missing(self) -> None:
        build_module = load_module(BUILD_MODULE_PATH, "chaos_run_build_reference_desktop_gate_refreshes_subject_exec")
        bootstrap = build_module.tooling_module.ToolBootstrapResult(
            ready=True,
            output="",
            errors=[],
        )
        allocated_dir = REPO_ROOT / "artifacts" / "presets" / "windows-x64-reference-test-run"
        run_id = "20260408-windows-0003"
        trace_output = REPO_ROOT / "artifacts" / "subjects" / "FixtureTraceSubject" / "runs" / run_id / "matrices" / "windows-reference-trace" / "runtime" / "trace.runtime.json"
        gate_record = REPO_ROOT / "artifacts" / "verify-roadmap-0" / "windows" / "windows-reference-desktop.gate.json"
        completed = subprocess.CompletedProcess(["cmd"], 0, stdout="ok", stderr="")
        execution_result = {
            "subjectId": "FixtureTraceSubject",
            "matrixId": "windows-reference-trace",
            "goalId": "correctness.platform",
            "status": "ok",
            "stageResults": [],
            "errors": [],
        }
        missing_generated = (
            REPO_ROOT
            / "artifacts"
            / "subjects"
            / "HelloWorldObject"
            / "runs"
            / "subject-exec"
            / "analysis"
            / "generated"
            / "generated"
            / "missing.cpp"
        )

        with patch.object(build_module.tooling_module, "ensure_dotnet_available", return_value=bootstrap):
            with patch.object(build_module, "allocate_run_scoped_binary_dir", return_value=allocated_dir):
                with patch.object(build_module, "run_process", return_value=completed):
                    with patch.object(
                        build_module.workspace_module,
                        "_subject_generated_source_path",
                        return_value=missing_generated,
                    ):
                        with patch.object(build_module.workspace_module, "refresh_subject_generated_root") as refresh_mock:
                            with patch.object(build_module.reporting_module, "build_run_id", return_value=run_id):
                                with patch.object(build_module.subject_executor_module, "execute_subject_matrix", return_value=execution_result):
                                    with patch.object(
                                        build_module.subject_executor_module,
                                        "trace_paths_from_execution",
                                        return_value=[
                                            "artifacts/subjects/FixtureTraceSubject/runs/20260408-windows-0003/matrices/windows-reference-trace/runtime/trace.runtime.json"
                                        ],
                                    ):
                                        result = build_module.handle(
                                            {
                                                "id": "build-platform-windows-reference-desktop",
                                                "kind": "reference-desktop-gate",
                                                "target": "windows-reference-desktop",
                                                "preset": "windows-x64-reference",
                                                "binary_dir": "artifacts/presets/windows-x64-reference",
                                                "subject_id": "FixtureTraceSubject",
                                                "goal_id": "correctness.platform",
                                                "matrix_id": "windows-reference-trace",
                                                "gate_record_path": "artifacts/verify-roadmap-0/windows/windows-reference-desktop.gate.json",
                                                "gate_name": "windows-reference-desktop",
                                                "gate_preset": "windows-x64-reference",
                                                "gate_notes": "Windows reference desktop gate passed with warmup trace compare.",
                                            },
                                            REPO_ROOT,
                                            "windows",
                                            "build platform windows-reference-desktop",
                                        )

        self.assertEqual("ok", result.status)
        self.assertEqual(
            [str(allocated_dir), str(trace_output), str(gate_record)],
            result.payload["artifacts"],
        )
        refresh_mock.assert_called_once_with(REPO_ROOT, "HelloWorldObject", "windows-dev-output", "CHECK")

    def test_platform_gate_uses_run_scoped_binary_dir(self) -> None:
        build_module = load_module(BUILD_MODULE_PATH, "chaos_run_build_platform_gate_scoped_dir_any_host")
        completed = subprocess.CompletedProcess(["cmake"], 0, stdout="", stderr="")
        requested_dir = REPO_ROOT / "artifacts" / "run" / "platform" / "linux-x64-packaging"
        allocated_dir = requested_dir.parent / "linux-x64-packaging-test-run"

        with patch.object(build_module, "allocate_run_scoped_binary_dir", return_value=allocated_dir):
            with patch.object(build_module, "run_process", return_value=completed) as run_process_mock:
                result = build_module.handle(
                    {
                        "id": "build-platform-linux-x64-packaging",
                        "kind": "platform-gate",
                        "target": "linux-x64-packaging",
                        "preset_target": "linux-x64-packaging",
                        "toolchain_file": "build/toolchains/linux-x64.cmake",
                        "binary_dir": "artifacts/run/platform/linux-x64-packaging",
                    },
                    REPO_ROOT,
                    "windows",
                    "build platform linux-x64-packaging",
                )

        self.assertEqual("ok", result.status)
        configure_args = run_process_mock.call_args_list[0].args[0]
        build_args = run_process_mock.call_args_list[1].args[0]
        self.assertIn(str(allocated_dir), configure_args)
        self.assertEqual(["--build", str(allocated_dir)], build_args[1:])
        self.assertEqual([str(allocated_dir)], result.payload["artifacts"])

    def test_build_failure_text_includes_underlying_output(self) -> None:
        build_module = load_module(BUILD_MODULE_PATH, "chaos_run_build_platform_gate_failure_text")
        completed = subprocess.CompletedProcess(["cmake"], 1, stdout="", stderr="toolchain validate only configure failed")

        with patch.object(build_module, "allocate_run_scoped_binary_dir", return_value=REPO_ROOT / "artifacts" / "run" / "platform" / "linux-x64-packaging-test-run"):
            with patch.object(build_module, "run_process", return_value=completed):
                result = build_module.handle(
                    {
                        "id": "build-platform-linux-x64-packaging",
                        "kind": "platform-gate",
                        "target": "linux-x64-packaging",
                        "preset_target": "linux-x64-packaging",
                        "toolchain_file": "build/toolchains/linux-x64.cmake",
                        "binary_dir": "artifacts/run/platform/linux-x64-packaging",
                    },
                    REPO_ROOT,
                    "windows",
                    "build platform linux-x64-packaging",
                )

        self.assertEqual("error", result.status)
        self.assertIn("toolchain validate only configure failed", result.payload["consoleText"])
        self.assertIn("toolchain validate only configure failed", result.text)


if __name__ == "__main__":
    unittest.main()
