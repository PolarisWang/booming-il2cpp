from __future__ import annotations

import importlib.util
import shutil
import subprocess
import sys
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
VERIFY_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "verify.py"
VERIFY_WRAPPER_PATH = REPO_ROOT / "build" / "scripts" / "verify-roadmap-0.sh"
VERIFY_SCRIPT_PATH = REPO_ROOT / "build" / "scripts" / "verify-roadmap-0.py"


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
        verify_module = load_module(VERIFY_MODULE_PATH, "booming_run_verify_route")
        bootstrap = verify_module.tooling_module.ToolBootstrapResult(
            ready=True,
            output="",
            errors=[],
        )
        completed = subprocess.CompletedProcess(["python3"], 0, stdout="verify ok", stderr="")

        with patch.object(verify_module.tooling_module, "ensure_dotnet_available", return_value=bootstrap):
            with patch.object(verify_module, "run_process", return_value=completed) as run_process_mock:
                result = verify_module.handle(
                    {"id": "roadmap-0-low-level-macos", "host_profile": "macos", "target": "macos"},
                    REPO_ROOT,
                    "macos",
                    "roadmap-0 low-level script (macos)",
                )

        args = run_process_mock.call_args.args[0]
        self.assertEqual("ok", result.status)
        self.assertEqual(sys.executable, args[0])
        self.assertEqual(REPO_ROOT / "build" / "scripts" / "verify-roadmap-0.py", Path(args[1]))

    def test_low_level_command_uses_execution_policy_bypass_on_windows(self) -> None:
        verify_module = load_module(VERIFY_MODULE_PATH, "booming_run_verify_route_windows")
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
                        {"id": "roadmap-0-low-level-windows", "host_profile": "windows", "target": "windows"},
                        REPO_ROOT,
                        "windows",
                        "roadmap-0 low-level script (windows)",
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
        self.assertEqual(REPO_ROOT / "build" / "scripts" / "verify-roadmap-0.ps1", Path(args[7]))
        self.assertEqual(["-HostProfile", "windows"], args[8:])

    def test_low_level_failure_text_includes_underlying_output(self) -> None:
        verify_module = load_module(VERIFY_MODULE_PATH, "booming_run_verify_route_failure_text")
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
                        {"id": "roadmap-0-low-level-windows", "host_profile": "windows", "target": "windows"},
                        REPO_ROOT,
                        "windows",
                        "roadmap-0 low-level script (windows)",
                    )

        self.assertEqual("error", result.status)
        self.assertIn("running scripts is disabled on this system", result.payload["consoleText"])
        self.assertIn("running scripts is disabled on this system", result.text)

    def test_low_level_script_allocates_run_scoped_binary_dir(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "booming_verify_roadmap0_script_paths")
        base_dir = REPO_ROOT / "artifacts" / "verify-roadmap-0" / "windows" / "common" / "native-abi-config"

        first = script_module.allocate_run_scoped_binary_dir(base_dir)
        second = script_module.allocate_run_scoped_binary_dir(base_dir)

        self.assertNotEqual(first, second)
        self.assertEqual(base_dir.parent, first.parent)
        self.assertTrue(first.name.startswith("native-abi-config-"))
        self.assertTrue(second.name.startswith("native-abi-config-"))

    def test_low_level_script_native_smoke_uses_run_scoped_binary_dir(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "booming_verify_roadmap0_script_native")
        source_dir = REPO_ROOT / "tests" / "contracts" / "native" / "abi"
        requested_dir = REPO_ROOT / "artifacts" / "verify-roadmap-0" / "windows" / "common" / "native-abi-config"
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

    def test_low_level_script_preset_build_uses_run_scoped_binary_dir(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "booming_verify_roadmap0_script_preset")
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

    def test_low_level_script_routing_build_uses_run_scoped_binary_dir(self) -> None:
        script_module = load_module(VERIFY_SCRIPT_PATH, "booming_verify_roadmap0_script_routing")
        requested_dir = REPO_ROOT / "artifacts" / "verify-roadmap-0" / "windows" / "common" / "linux-packaging-routing"
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


if __name__ == "__main__":
    unittest.main()
