from __future__ import annotations

import importlib.util
import shutil
import subprocess
import sys
import tempfile
import unittest
import uuid
from pathlib import Path
from unittest.mock import patch

from tests.support import select_public_suite_spec


REPO_ROOT = Path(__file__).resolve().parents[3]
VERIFY_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "verify.py"
VERIFY_WRAPPER_PATH = REPO_ROOT / "build" / "scripts" / "verify-runtime-baseline.sh"
VERIFY_SCRIPT_PATH = REPO_ROOT / "build" / "scripts" / "verify-runtime-baseline.py"
VERIFY_SCRIPT_PS1_PATH = REPO_ROOT / "build" / "scripts" / "verify-runtime-baseline.ps1"
LEGACY_VERIFY_WRAPPER_PATH = REPO_ROOT / "build" / "scripts" / "verify-roadmap-0.sh"
LEGACY_VERIFY_SCRIPT_PS1_PATH = REPO_ROOT / "build" / "scripts" / "verify-roadmap-0.ps1"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "verify-roadmap0"


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


class Roadmap0LowLevelScriptTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_test_dir(self, prefix: str) -> Path:
        path = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        path.mkdir(parents=True, exist_ok=False)
        return path

    def test_posix_wrapper_runs_without_pwsh(self) -> None:
        if shutil.which("sh") is None:
            self.skipTest("POSIX shell is not available in this environment")
        completed = subprocess.run(
            ["sh", str(VERIFY_WRAPPER_PATH), "--help"],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
        )

        self.assertEqual(0, completed.returncode, msg=completed.stdout + completed.stderr)
        self.assertIn("host-profile", completed.stdout)
        self.assertNotIn("pwsh", completed.stderr)

    def test_low_level_command_uses_python_entrypoint_on_macos(self) -> None:
        verify_module = load_module(VERIFY_MODULE_PATH, "chaos_run_verify_route")
        bootstrap = verify_module.tooling_module.ToolBootstrapResult(
            ready=True,
            output="",
            errors=[],
        )
        completed = subprocess.CompletedProcess(["python3"], 0, stdout="verify ok", stderr="")

        with patch.object(verify_module.tooling_module, "ensure_dotnet_available", return_value=bootstrap):
            with patch.object(verify_module, "run_process", return_value=completed) as run_process_mock:
                result = verify_module.handle(
                    {"id": "runtime-baseline-low-level-macos", "host_profile": "macos", "target": "macos"},
                    REPO_ROOT,
                    "macos",
                    "runtime-baseline low-level script (macos)",
                )

        args = run_process_mock.call_args.args[0]
        self.assertEqual("ok", result.status)
        self.assertEqual(sys.executable, args[0])
        self.assertEqual(REPO_ROOT / "build" / "scripts" / "verify-runtime-baseline.py", Path(args[1]))

    def test_low_level_command_uses_execution_policy_bypass_on_windows(self) -> None:
        verify_module = load_module(VERIFY_MODULE_PATH, "chaos_run_verify_route_windows")
        bootstrap = verify_module.tooling_module.ToolBootstrapResult(
            ready=True,
            output="",
            errors=[],
        )
        completed = subprocess.CompletedProcess(["powershell"], 0, stdout="verify ok", stderr="")

        with patch.object(verify_module.tooling_module, "ensure_dotnet_available", return_value=bootstrap):
            with patch.object(verify_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                with patch.object(verify_module, "run_process", return_value=completed) as run_process_mock:
                    result = verify_module.handle(
                        {"id": "runtime-baseline-low-level-windows", "host_profile": "windows", "target": "windows"},
                        REPO_ROOT,
                        "windows",
                        "runtime-baseline low-level script (windows)",
                    )

        args = run_process_mock.call_args.args[0]
        self.assertEqual("ok", result.status)
        self.assertEqual(
            [
                "powershell",
                "-NoLogo",
                "-NoProfile",
                "-NonInteractive",
                "-ExecutionPolicy",
                "Bypass",
                "-File",
            ],
            args[:7],
        )
        self.assertEqual(REPO_ROOT / "build" / "scripts" / "verify-runtime-baseline.ps1", Path(args[7]))
        self.assertEqual(["-HostProfile", "windows"], args[8:])

    def test_low_level_failure_text_includes_underlying_output(self) -> None:
        verify_module = load_module(VERIFY_MODULE_PATH, "chaos_run_verify_route_failure_text")
        bootstrap = verify_module.tooling_module.ToolBootstrapResult(
            ready=True,
            output="",
            errors=[],
        )
        completed = subprocess.CompletedProcess(
            ["powershell"],
            1,
            stdout="",
            stderr="running scripts is disabled on this system",
        )

        with patch.object(verify_module.tooling_module, "ensure_dotnet_available", return_value=bootstrap):
            with patch.object(verify_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
                with patch.object(verify_module, "run_process", return_value=completed):
                    result = verify_module.handle(
                        {"id": "runtime-baseline-low-level-windows", "host_profile": "windows", "target": "windows"},
                        REPO_ROOT,
                        "windows",
                        "runtime-baseline low-level script (windows)",
                    )

        self.assertEqual("error", result.status)
        self.assertIn("running scripts is disabled on this system", result.payload["consoleText"])
        self.assertIn("running scripts is disabled on this system", result.text)

    def test_low_level_script_allocates_run_scoped_binary_dir(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_paths")
        base_dir = REPO_ROOT / "artifacts" / "verify-runtime-baseline" / "windows" / "common" / "native-abi-config"

        first = script_module.allocate_run_scoped_binary_dir(base_dir)
        second = script_module.allocate_run_scoped_binary_dir(base_dir)

        self.assertNotEqual(first, second)
        self.assertEqual(base_dir.parent, first.parent)
        self.assertTrue(first.name.startswith("native-abi-config-"))
        self.assertTrue(second.name.startswith("native-abi-config-"))

    def test_low_level_script_native_smoke_uses_run_scoped_binary_dir(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_native")
        source_dir = REPO_ROOT / "tests" / "contracts" / "native" / "abi"
        requested_dir = REPO_ROOT / "artifacts" / "verify-runtime-baseline" / "windows" / "common" / "native-abi-config"
        allocated_dir = requested_dir.parent / "native-abi-config-test-run"

        with patch.object(script_module, "allocate_run_scoped_binary_dir", return_value=allocated_dir):
            with patch.object(script_module, "run_checked") as run_checked_mock:
                script_module.invoke_native_smoke_build(source_dir, requested_dir, REPO_ROOT)

        self.assertEqual(
            ["cmake", "-S", str(source_dir), "-B", str(allocated_dir)],
            run_checked_mock.call_args_list[0].args[0],
        )
        self.assertEqual(
            ["cmake", "--build", str(allocated_dir)],
            run_checked_mock.call_args_list[1].args[0],
        )

    def test_low_level_script_native_smoke_uses_visual_studio_compatible_binary_dir_on_windows(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_native_windows_vs")
        source_dir = REPO_ROOT / "tests" / "contracts" / "native" / "abi"
        requested_dir = REPO_ROOT / "artifacts" / "verify-runtime-baseline" / "windows" / "common" / "native-abi-config"
        allocated_dir = TEST_TMP_ROOT / "cmake-builds" / "native-abi-config-1234"
        instance_spec = f"{TEST_TMP_ROOT / 'visual-studio' / '18' / 'Professional'},version=18.4.11626.88"

        with patch.object(script_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
            with patch.object(script_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                with patch.object(script_module.tooling_module, "allocate_cmake_binary_dir", return_value=allocated_dir) as allocate_mock:
                    with patch.object(script_module, "run_checked") as run_checked_mock:
                        script_module.invoke_native_smoke_build(
                            source_dir,
                            requested_dir,
                            REPO_ROOT,
                            host_profile="windows",
                        )

        allocate_mock.assert_called_once_with(
            requested_dir,
            host_platform="windows",
            generator="Visual Studio 18 2026",
        )
        self.assertEqual(
            [
                "cmake",
                "-S",
                str(source_dir),
                "-B",
                str(allocated_dir),
                "-G",
                "Visual Studio 18 2026",
                f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}",
            ],
            run_checked_mock.call_args_list[0].args[0],
        )

    def test_low_level_script_preset_build_uses_run_scoped_binary_dir(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_preset")
        requested_dir = REPO_ROOT / "artifacts" / "presets" / "windows-x64-reference"
        allocated_dir = requested_dir.parent / "windows-x64-reference-test-run"

        with patch.object(script_module, "allocate_run_scoped_binary_dir", return_value=allocated_dir):
            with patch.object(script_module, "run_checked") as run_checked_mock:
                script_module.invoke_preset_build_smoke("windows-x64-reference", REPO_ROOT)

        self.assertEqual(
            ["cmake", "--preset", "windows-x64-reference", "-B", str(allocated_dir)],
            run_checked_mock.call_args_list[0].args[0],
        )
        self.assertEqual(
            ["cmake", "--build", str(allocated_dir)],
            run_checked_mock.call_args_list[1].args[0],
        )

    def test_low_level_script_dotnet_build_uses_temp_intermediate_root_on_windows(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_dotnet_build")
        smoke_spec = select_public_suite_spec(
            "chaos_verify_roadmap0_dotnet_build_suite",
            host_platform="windows",
            family="smoke",
            required_stages=["build"],
        )
        suite_name = str(smoke_spec["suite"])
        project_path = REPO_ROOT / "subjects" / suite_name / "source" / f"{suite_name}.csproj"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / f"{suite_name}-1234"

        with patch.object(script_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
            with patch.object(script_module, "run_checked") as run_checked_mock:
                script_module.invoke_dotnet_build(project_path, REPO_ROOT, host_profile="windows")

        self.assertEqual(
            [
                "dotnet",
                "build",
                str(project_path),
                "-c",
                "Release",
                f"-p:BaseIntermediateOutputPath={intermediate_root.as_posix()}/$(MSBuildProjectName)/",
                f"-p:MSBuildProjectExtensionsPath={intermediate_root.as_posix()}/$(MSBuildProjectName)/",
            ],
            run_checked_mock.call_args.args[0],
        )

    def test_low_level_script_routing_build_uses_run_scoped_binary_dir(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_routing")
        requested_dir = REPO_ROOT / "artifacts" / "verify-runtime-baseline" / "windows" / "common" / "linux-packaging-routing"
        allocated_dir = requested_dir.parent / "linux-packaging-routing-test-run"

        with patch.object(script_module, "allocate_run_scoped_binary_dir", return_value=allocated_dir):
            with patch.object(script_module, "run_checked") as run_checked_mock:
                script_module.invoke_routing_build_smoke(
                    "linux-x64-packaging",
                    REPO_ROOT / "build" / "toolchains" / "linux-x64.cmake",
                    requested_dir,
                    "Ninja",
                    REPO_ROOT,
                )

        self.assertEqual(
            [
                "cmake",
                "-S",
                str(REPO_ROOT),
                "-B",
                str(allocated_dir),
                "-G",
                "Ninja",
                "-DROADMAP0_PRESET_TARGET=linux-x64-packaging",
                "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON",
                f"-DCMAKE_TOOLCHAIN_FILE={REPO_ROOT / 'build' / 'toolchains' / 'linux-x64.cmake'}",
            ],
            run_checked_mock.call_args_list[0].args[0],
        )
        self.assertEqual(
            ["cmake", "--build", str(allocated_dir)],
            run_checked_mock.call_args_list[1].args[0],
        )

    def test_low_level_script_execute_subject_matrix_builds_plan_and_executes_it(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_subject_matrix")
        plan = {"selection": {"artifactPlan": {"evidenceTerminalBucket": "runtime"}}}
        result = {"status": "ok", "errors": [], "stageResults": []}

        with patch.object(script_module, "resolve_subject_matrix_subject_id", return_value="FixtureReferenceSubject"):
            with patch.object(script_module, "build_subject_run_id", return_value="fixture-run-001"):
                with patch.object(script_module.subject_planner_module, "build_plan", return_value=plan) as build_plan_mock:
                    with patch.object(script_module.subject_executor_module, "execute_plan", return_value=result) as execute_plan_mock:
                        actual = script_module.execute_subject_matrix(
                            REPO_ROOT,
                            matrix_id="windows-dev-output",
                            goal_id="correctness.dev",
                        )

        self.assertEqual({**result, "runId": "fixture-run-001"}, actual)
        build_plan_mock.assert_called_once_with(
            REPO_ROOT,
            "FixtureReferenceSubject",
            goal_id="correctness.dev",
            matrix_id="windows-dev-output",
            run_id="fixture-run-001",
        )
        execute_plan_mock.assert_called_once_with(REPO_ROOT, plan, run_id="fixture-run-001")

    def test_low_level_script_raises_when_subject_matrix_execution_fails(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_subject_matrix_fail")
        plan = {"selection": {"artifactPlan": {"evidenceTerminalBucket": "runtime"}}}
        result = {"status": "fail", "errors": ["worker boom"], "stageResults": []}

        with patch.object(script_module.subject_planner_module, "build_plan", return_value=plan):
            with patch.object(script_module.subject_executor_module, "execute_plan", return_value=result):
                with self.assertRaisesRegex(RuntimeError, "subject matrix failed: windows-dev-output"):
                    script_module.execute_subject_matrix(
                        REPO_ROOT,
                        matrix_id="windows-dev-output",
                        goal_id="correctness.dev",
                    )

    def test_low_level_script_accepts_stage4_proof_run_artifacts_when_runtime_root_contains_expected_files(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_stage4_run_artifacts_ok")

        runtime_root = self._make_test_dir("stage4-runtime-ok")
        try:
            (runtime_root / "stdout.log").write_text("Hello, World!\n", encoding="utf-8")
            (runtime_root / "stderr.log").write_text("", encoding="utf-8")
            (runtime_root / "exit-code.txt").write_text("0\n", encoding="utf-8")

            script_module.validate_stage4_proof_run_artifacts(runtime_root)
        finally:
            shutil.rmtree(runtime_root, ignore_errors=True)

    def test_low_level_script_accepts_stage4_proof_run_artifacts_when_legacy_root_contains_run_subdir(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_stage4_run_artifacts_legacy_ok")

        legacy_root = self._make_test_dir("stage4-runtime-legacy")
        try:
            run_root = legacy_root / "run"
            run_root.mkdir(parents=True, exist_ok=True)
            (run_root / "stdout.log").write_text("Hello, World!\n", encoding="utf-8")
            (run_root / "stderr.log").write_text("", encoding="utf-8")
            (run_root / "exit-code.txt").write_text("0\n", encoding="utf-8")

            script_module.validate_stage4_proof_run_artifacts(legacy_root)
        finally:
            shutil.rmtree(legacy_root, ignore_errors=True)

    def test_low_level_script_rejects_stage4_proof_run_exit_code_mismatch(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_stage4_run_artifacts_exit")

        runtime_root = self._make_test_dir("stage4-runtime-exit")
        try:
            (runtime_root / "stdout.log").write_text("Hello, World!\n", encoding="utf-8")
            (runtime_root / "stderr.log").write_text("", encoding="utf-8")
            (runtime_root / "exit-code.txt").write_text("1\n", encoding="utf-8")

            with self.assertRaisesRegex(RuntimeError, "exit code"):
                script_module.validate_stage4_proof_run_artifacts(runtime_root)
        finally:
            shutil.rmtree(runtime_root, ignore_errors=True)

    def test_low_level_script_rejects_stage4_proof_run_stdout_mismatch(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "chaos_verify_roadmap0_script_stage4_run_artifacts_stdout")

        runtime_root = self._make_test_dir("stage4-runtime-stdout")
        try:
            (runtime_root / "stdout.log").write_text("unexpected output\n", encoding="utf-8")
            (runtime_root / "stderr.log").write_text("", encoding="utf-8")
            (runtime_root / "exit-code.txt").write_text("0\n", encoding="utf-8")

            with self.assertRaisesRegex(RuntimeError, "Hello, World!"):
                script_module.validate_stage4_proof_run_artifacts(runtime_root)
        finally:
            shutil.rmtree(runtime_root, ignore_errors=True)

    def test_low_level_powershell_script_is_a_python_forwarder(self) -> None:
        script_text = VERIFY_SCRIPT_PS1_PATH.read_text(encoding="utf-8")

        required_markers = [
            "Get-Command python",
            "Get-Command py",
            "verify-runtime-baseline.py",
            "--host-profile",
        ]
        forbidden_markers = [
            "Assert-Stage4ProofRunArtifacts",
            "Invoke-Stage4NativeReferenceProofRun",
            "Invoke-Stage4NativeReferenceCodegen",
        ]

        for marker in required_markers:
            self.assertIn(marker, script_text)
        for marker in forbidden_markers:
            self.assertNotIn(marker, script_text)

    def test_legacy_powershell_wrapper_forwards_to_runtime_baseline_script(self) -> None:
        script_text = LEGACY_VERIFY_SCRIPT_PS1_PATH.read_text(encoding="utf-8")

        self.assertIn("verify-runtime-baseline.py", script_text)

    def test_legacy_shell_wrapper_forwards_to_runtime_baseline_script(self) -> None:
        script_text = LEGACY_VERIFY_WRAPPER_PATH.read_text(encoding="utf-8")

        self.assertIn("verify-runtime-baseline.py", script_text)


if __name__ == "__main__":
    unittest.main()
