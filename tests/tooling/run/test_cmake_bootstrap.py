from __future__ import annotations

import importlib.util
import subprocess
import sys
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
TOOLING_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "tooling.py"


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


class CMakeBootstrapTests(unittest.TestCase):
    def test_find_cmake_executable_discovers_repo_cached_pip_layout(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "booming_run_tooling_find_cached_cmake")
        temp_dir = REPO_ROOT / "artifacts" / ".tmp-tests" / "cmake-find-layout"
        if temp_dir.exists():
            tooling_module.shutil.rmtree(temp_dir, ignore_errors=True)
        temp_dir.mkdir(parents=True, exist_ok=True)

        repo_root = temp_dir
        cached_cmake = repo_root / "artifacts" / "toolchains" / "cmake" / "cmake" / "data" / "bin" / "cmake.exe"
        cached_cmake.parent.mkdir(parents=True, exist_ok=True)
        cached_cmake.write_text("", encoding="utf-8")

        discovered = tooling_module.find_cmake_executable(repo_root, which=lambda executable: None)

        self.assertEqual(str(cached_cmake), discovered)

    def test_windows_session_can_bootstrap_cmake_into_repo_cache(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "booming_run_tooling_install_cmake")
        cached_cmake = REPO_ROOT / "artifacts" / "toolchains" / "cmake" / "cmake" / "data" / "bin" / "cmake.exe"
        commands: list[list[str]] = []
        find_calls = 0

        def fake_find(repo_root: Path | None = None, which=None) -> str | None:
            del repo_root, which
            nonlocal find_calls
            find_calls += 1
            return None if find_calls == 1 else str(cached_cmake)

        def fake_which(executable: str) -> str | None:
            if executable == "python":
                return r"C:\Python314\python.exe"
            return None

        def fake_run(arguments: list[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
            del cwd
            commands.append(arguments)
            if arguments[-2:] == ["pip", "--version"]:
                return subprocess.CompletedProcess(arguments, 0, "pip 25.3", "")
            return subprocess.CompletedProcess(arguments, 0, "cmake install completed", "")

        outcome = tooling_module.ensure_cmake_available(
            "prepare",
            "windows",
            REPO_ROOT,
            which=fake_which,
            run=fake_run,
            find_cmake=fake_find,
        )

        self.assertTrue(outcome.ready)
        self.assertIn("cmake install completed", outcome.output)
        self.assertIn(str(cached_cmake), outcome.output)
        self.assertEqual([r"C:\Python314\python.exe", "-m", "pip", "--version"], commands[0])
        self.assertEqual(
            [
                r"C:\Python314\python.exe",
                "-m",
                "pip",
                "install",
                "--disable-pip-version-check",
                "--upgrade",
                "--target",
                str(REPO_ROOT / "artifacts" / "toolchains" / "cmake"),
                "cmake",
            ],
            commands[1],
        )

    def test_detect_visual_studio_generator_prefers_first_available_entry(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "booming_run_tooling_detect_vs_generator")
        help_output = """
Generators

The following generators are available on this platform (* marks default):
* Visual Studio 18 2026        = Generates Visual Studio 2026 project files.
  Visual Studio 17 2022        = Generates Visual Studio 2022 project files.
  Ninja                        = Generates build.ninja files.
"""
        completed = subprocess.CompletedProcess(["cmake"], 0, help_output, "")

        def fake_run(arguments: list[str], cwd: Path | None = None, env: dict[str, str] | None = None) -> subprocess.CompletedProcess[str]:
            del cwd, env
            self.assertEqual(["cmake", "--help"], arguments)
            return completed

        detected = tooling_module.detect_visual_studio_generator("cmake", run=fake_run)

        self.assertEqual("Visual Studio 18 2026", detected)

    def test_detect_visual_studio_instance_spec_prefers_matching_major_version(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "booming_run_tooling_detect_vs_instance")

        with patch.object(
            tooling_module,
            "_candidate_visual_studio_instance_specs",
            return_value=[
                (Path(r"C:\Program Files\Microsoft Visual Studio\17\Community"), "17.9.34728.123"),
                (Path(r"C:\Program Files\Microsoft Visual Studio\18\Professional"), "18.4.11626.88"),
            ],
        ):
            detected = tooling_module.detect_visual_studio_instance_spec("Visual Studio 18 2026")

        self.assertEqual(
            r"C:\Program Files\Microsoft Visual Studio\18\Professional,version=18.4.11626.88",
            detected,
        )

    def test_allocate_cmake_binary_dir_uses_temp_root_for_windows_visual_studio(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "booming_run_tooling_allocate_vs_binary_dir")
        base_dir = REPO_ROOT / "artifacts" / "verify-roadmap-0" / "windows" / "common" / "native-abi-config"
        expected_dir = Path(r"C:\Users\mayna\AppData\Local\Temp\booming-native-abi-config-4242-a1b2c3d4")

        with patch.object(tooling_module.tempfile, "gettempdir", return_value=r"C:\Users\mayna\AppData\Local\Temp"):
            with patch.object(tooling_module.os, "getpid", return_value=4242):
                with patch.object(tooling_module.uuid, "uuid4") as uuid4_mock:
                    uuid4_mock.return_value.hex = "a1b2c3d4e5f60708"
                    with patch.object(Path, "mkdir") as mkdir_mock:
                        actual = tooling_module.allocate_cmake_binary_dir(
                            base_dir,
                            host_platform="windows",
                            generator="Visual Studio 18 2026",
                        )

        self.assertEqual(expected_dir, actual)
        mkdir_mock.assert_called_once_with(parents=True, exist_ok=False)


if __name__ == "__main__":
    unittest.main()
