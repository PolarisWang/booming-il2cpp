from __future__ import annotations

import importlib.util
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


@unittest.skipUnless(subprocess.run(["uname"], capture_output=True, text=True).stdout.strip() == "Darwin", "macOS only")
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
        build_module = load_module(BUILD_MODULE_PATH, "booming_run_build_platform_gate")
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
        self.assertIn("Ninja", configure_args)

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
        self.assertIn("Ninja", configure_args)


if __name__ == "__main__":
    unittest.main()
