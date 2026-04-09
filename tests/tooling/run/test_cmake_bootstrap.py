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
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_find_cached_cmake")
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
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_install_cmake")
        cached_cmake = REPO_ROOT / "artifacts" / "toolchains" / "cmake" / "cmake" / "data" / "bin" / "cmake.exe"
        fake_python = REPO_ROOT / "artifacts" / ".tmp-tests" / "tooling-python" / "python.exe"
        commands: list[list[str]] = []
        find_calls = 0

        def fake_find(repo_root: Path | None = None, which=None) -> str | None:
            del repo_root, which
            nonlocal find_calls
            find_calls += 1
            return None if find_calls == 1 else str(cached_cmake)

        def fake_which(executable: str) -> str | None:
            if executable == "python":
                return str(fake_python)
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
        self.assertEqual([str(fake_python), "-m", "pip", "--version"], commands[0])
        self.assertEqual(
            [
                str(fake_python),
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
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_detect_vs_generator")
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
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_detect_vs_instance")
        visual_studio_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "tooling-visual-studio"

        with patch.object(
            tooling_module,
            "_candidate_visual_studio_instance_specs",
            return_value=[
                (visual_studio_root / "17" / "Community", "17.9.34728.123"),
                (visual_studio_root / "18" / "Professional", "18.4.11626.88"),
            ],
        ):
            detected = tooling_module.detect_visual_studio_instance_spec("Visual Studio 18 2026")

        self.assertEqual(
            f"{visual_studio_root / '18' / 'Professional'},version=18.4.11626.88",
            detected,
        )

    def test_detect_visual_studio_instance_spec_accepts_visual_studio_year_layout(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_detect_vs_instance_year_layout")
        visual_studio_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "tooling-visual-studio-year"

        with patch.object(
            tooling_module,
            "_candidate_visual_studio_instance_specs",
            return_value=[
                (visual_studio_root / "2019" / "Community", "16.11.34407.143"),
                (visual_studio_root / "2022" / "Professional", "17.12.35527.113"),
            ],
        ):
            detected = tooling_module.detect_visual_studio_instance_spec("Visual Studio 17 2022")

        self.assertEqual(
            f"{visual_studio_root / '2022' / 'Professional'},version=17.12.35527.113",
            detected,
        )

    def test_find_visual_studio_developer_command_prefers_vswhere_result(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_find_vsdevcmd")
        developer_command = REPO_ROOT / "artifacts" / ".tmp-tests" / "tooling-visual-studio" / "2022" / "Professional" / "Common7" / "Tools" / "VsDevCmd.bat"

        with patch.object(tooling_module, "_run_vswhere", return_value=[str(developer_command)]) as run_vswhere_mock:
            detected = tooling_module.find_visual_studio_developer_command()

        self.assertEqual(developer_command, detected)
        run_vswhere_mock.assert_called_once_with(
            [
                "-latest",
                "-products",
                "*",
                "-requires",
                "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
                "-find",
                r"Common7\Tools\VsDevCmd.bat",
            ]
        )

    def test_find_visual_cpp_executable_prefers_developer_environment_toolset(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_find_cl_from_dev_env")
        toolset_root = (
            REPO_ROOT
            / "artifacts"
            / ".tmp-tests"
            / "tooling-visual-studio"
            / "2022"
            / "Professional"
            / "VC"
            / "Tools"
            / "MSVC"
            / "14.38.33130"
        )
        expected = toolset_root / "bin" / "Hostx64" / "x64" / "cl.exe"

        with patch.object(tooling_module.os, "name", "nt"):
            with patch.object(
                tooling_module,
                "windows_developer_environment",
                return_value={"VCToolsInstallDir": str(toolset_root) + "\\"},
            ):
                with patch.object(Path, "is_file", autospec=True) as is_file_mock:
                    is_file_mock.side_effect = lambda path: path == expected
                    detected = tooling_module.find_visual_cpp_executable(
                        which=lambda executable: r"C:\toolchains\legacy\cl.exe" if executable == "cl" else None
                    )

        self.assertEqual(str(expected), detected)

    def test_find_visual_cpp_executable_prefers_highest_vswhere_toolset_match(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_find_cl_latest_vswhere")
        older = (
            REPO_ROOT
            / "artifacts"
            / ".tmp-tests"
            / "tooling-visual-studio"
            / "2022"
            / "Professional"
            / "VC"
            / "Tools"
            / "MSVC"
            / "14.29.30133"
            / "bin"
            / "Hostx64"
            / "x64"
            / "cl.exe"
        )
        newer = (
            REPO_ROOT
            / "artifacts"
            / ".tmp-tests"
            / "tooling-visual-studio"
            / "2022"
            / "Professional"
            / "VC"
            / "Tools"
            / "MSVC"
            / "14.38.33130"
            / "bin"
            / "Hostx64"
            / "x64"
            / "cl.exe"
        )

        with patch.object(tooling_module.os, "name", "nt"):
            with patch.object(tooling_module, "windows_developer_environment", return_value={}):
                with patch.object(tooling_module, "_run_vswhere", return_value=[str(older), str(newer)]):
                    detected = tooling_module.find_visual_cpp_executable(which=lambda executable: None)

        self.assertEqual(str(newer), detected)

    def test_find_ninja_executable_prefers_visual_studio_cmake_bundle(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_find_ninja")
        install_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "tooling-visual-studio" / "2022" / "Professional"
        bundled_ninja = install_root / "Common7" / "IDE" / "CommonExtensions" / "Microsoft" / "CMake" / "Ninja" / "ninja.exe"

        with patch.object(
            tooling_module,
            "_candidate_visual_studio_install_paths",
            return_value=[install_root],
        ):
            with patch.object(Path, "is_file", autospec=True) as is_file_mock:
                def fake_is_file(path: Path) -> bool:
                    return path == bundled_ninja

                is_file_mock.side_effect = fake_is_file
                detected = tooling_module.find_ninja_executable(which=lambda executable: None)

        self.assertEqual(str(bundled_ninja), detected)

    def test_windows_developer_environment_parses_vsdevcmd_set_output(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_windows_dev_env")
        developer_command = REPO_ROOT / "artifacts" / ".tmp-tests" / "tooling-visual-studio" / "2022" / "Professional" / "Common7" / "Tools" / "VsDevCmd.bat"
        completed = subprocess.CompletedProcess(
            ["cmd.exe"],
            0,
            "INCLUDE=C:\\VS\\include\nLIB=C:\\VS\\lib\nPATH=C:\\VS\\bin;C:\\VS\\ninja;C:\\Windows\\System32\nPath=C:\\Windows\\System32\n",
            "",
        )

        with patch.object(tooling_module, "find_visual_studio_developer_command", return_value=developer_command):
            with patch.object(tooling_module.subprocess, "run", return_value=completed) as run_mock:
                env = tooling_module.windows_developer_environment()

        self.assertEqual(
            {
                "INCLUDE": r"C:\VS\include",
                "LIB": r"C:\VS\lib",
                "PATH": r"C:\VS\bin;C:\VS\ninja;C:\Windows\System32",
            },
            env,
        )
        self.assertEqual(
            f'call "{developer_command}" -arch=x64 -host_arch=x64 >nul && set',
            run_mock.call_args.args[0],
        )
        self.assertTrue(run_mock.call_args.kwargs["shell"])
        self.assertTrue(run_mock.call_args.kwargs["capture_output"])
        self.assertTrue(run_mock.call_args.kwargs["text"])
        self.assertEqual("replace", run_mock.call_args.kwargs["errors"])
        self.assertFalse(run_mock.call_args.kwargs["check"])

    def test_allocate_cmake_binary_dir_uses_temp_root_for_windows_visual_studio(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_allocate_vs_binary_dir")
        base_dir = REPO_ROOT / "artifacts" / "verify-roadmap-0" / "windows" / "common" / "native-abi-config"
        temp_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "tooling-temp-root"
        expected_dir = temp_root / "chaos-native-abi-config-4242-a1b2c3d4"

        with patch.object(tooling_module.tempfile, "gettempdir", return_value=str(temp_root)):
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
