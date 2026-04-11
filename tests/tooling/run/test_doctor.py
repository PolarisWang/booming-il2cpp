from __future__ import annotations

import importlib.util
import os
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
DOCTOR_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "doctor.py"


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


class DoctorCommandTests(unittest.TestCase):
    def test_doctor_highlights_status_prefixes(self) -> None:
        doctor_module = load_module(DOCTOR_MODULE_PATH, "chaos_run_doctor_status_highlight")

        with patch.object(
            doctor_module.runtime_module,
            "probe_runtime",
            return_value={"isInstalled": True, "pythonPath": "artifacts/python/python.exe"},
        ):
            with patch.dict(os.environ, {"TERM": "dumb"}, clear=True):
                with patch.object(doctor_module.tooling_module, "find_cmake_executable", return_value="C:\\tools\\cmake\\bin\\cmake.exe"):
                    with patch.object(doctor_module.tooling_module, "find_visual_cpp_executable", return_value=None):
                        with patch.object(
                            doctor_module.tooling_module.shutil,
                            "which",
                            side_effect=lambda exe: "C:\\Program Files\\dotnet\\dotnet.exe" if exe == "dotnet" else None,
                        ):
                            result = doctor_module.handle(REPO_ROOT, "windows", "doctor")

        self.assertIn("\x1b[1;32m[ok]\x1b[0m cached-python-runtime", result.text or "")
        self.assertIn("\x1b[1;36m[note]\x1b[0m osc8-hyperlinks", result.text or "")

    def test_doctor_reports_osc8_support_when_terminal_looks_capable(self) -> None:
        doctor_module = load_module(DOCTOR_MODULE_PATH, "chaos_run_doctor_osc8_supported")

        with patch.object(
            doctor_module.runtime_module,
            "probe_runtime",
            return_value={"isInstalled": True, "pythonPath": "artifacts/python/bin/python3"},
        ):
            with patch.object(doctor_module.tooling_module, "find_cmake_executable", return_value="/usr/bin/cmake"):
                with patch.object(doctor_module.tooling_module, "find_visual_cpp_executable", return_value="/usr/bin/cl"):
                    with patch.object(doctor_module.tooling_module.shutil, "which", side_effect=lambda exe: f"/usr/bin/{exe}"):
                        with patch.dict(
                            os.environ,
                            {
                                "TERM_PROGRAM": "iTerm.app",
                                "TERM": "xterm-256color",
                            },
                            clear=True,
                        ):
                            result = doctor_module.handle(REPO_ROOT, "macos", "doctor")

        self.assertEqual("ok", result.status)
        osc8_check = next(check for check in result.checks if check["name"] == "osc8-hyperlinks")
        self.assertEqual("ok", osc8_check["status"])
        self.assertIn("supported", osc8_check["detail"])
        self.assertIn("Summary:", result.text or "")
        self.assertIn("Required checks:", result.text or "")
        self.assertIn("Current terminal looks compatible", result.text or "")

    def test_doctor_explains_that_osc8_is_not_installable_when_support_is_unknown(self) -> None:
        doctor_module = load_module(DOCTOR_MODULE_PATH, "chaos_run_doctor_osc8_unknown")

        with patch.object(
            doctor_module.runtime_module,
            "probe_runtime",
            return_value={"isInstalled": True, "pythonPath": "artifacts/python/bin/python3"},
        ):
            with patch.object(doctor_module.tooling_module, "find_cmake_executable", return_value="/usr/bin/cmake"):
                with patch.object(doctor_module.tooling_module, "find_visual_cpp_executable", return_value=None):
                    with patch.object(doctor_module.tooling_module.shutil, "which", side_effect=lambda exe: f"/usr/bin/{exe}"):
                        with patch.dict(
                            os.environ,
                            {
                                "TERM_PROGRAM": "Apple_Terminal",
                                "TERM": "xterm-256color",
                            },
                            clear=True,
                        ):
                            result = doctor_module.handle(REPO_ROOT, "macos", "doctor")

        self.assertEqual("ok", result.status)
        osc8_check = next(check for check in result.checks if check["name"] == "osc8-hyperlinks")
        self.assertEqual("missing", osc8_check["status"])
        self.assertIn("not installable", osc8_check["detail"])
        self.assertIn("cannot be installed by run doctor", result.text or "")
        self.assertIn("Recommended terminals", result.text or "")
        self.assertIn("iTerm2", result.text or "")
        self.assertIn("WezTerm", result.text or "")
        self.assertIn("Apple_Terminal", result.text or "")

    def test_doctor_marks_visual_cpp_toolchain_ok_when_discovered_via_visual_studio(self) -> None:
        doctor_module = load_module(DOCTOR_MODULE_PATH, "chaos_run_doctor_windows_vswhere")

        with patch.object(
            doctor_module.runtime_module,
            "probe_runtime",
            return_value={"isInstalled": True, "pythonPath": "artifacts/python/python.exe"},
        ):
            with patch.object(doctor_module.tooling_module, "find_cmake_executable", return_value="C:\\tools\\cmake\\bin\\cmake.exe"):
                with patch.object(
                    doctor_module.tooling_module,
                    "find_visual_cpp_executable",
                    return_value="D:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Tools\\MSVC\\14.42.34433\\bin\\Hostx64\\x64\\cl.exe",
                ):
                    with patch.object(
                        doctor_module.tooling_module.shutil,
                        "which",
                        side_effect=lambda exe: "C:\\Program Files\\dotnet\\dotnet.exe" if exe == "dotnet" else None,
                    ):
                        result = doctor_module.handle(REPO_ROOT, "windows", "doctor")

        self.assertEqual("ok", result.status)
        vc_check = next(check for check in result.checks if check["name"] == "visual-cpp-toolchain")
        self.assertEqual("ok", vc_check["status"])
        self.assertIn("discovered via Visual Studio", vc_check["detail"])

    def test_doctor_guides_windows_users_to_prepare_when_cmake_is_missing(self) -> None:
        doctor_module = load_module(DOCTOR_MODULE_PATH, "chaos_run_doctor_missing_cmake_prepare_guidance")

        with patch.object(
            doctor_module.runtime_module,
            "probe_runtime",
            return_value={"isInstalled": True, "pythonPath": "artifacts/python/python.exe"},
        ):
            with patch.object(doctor_module.tooling_module, "find_cmake_executable", return_value=None):
                with patch.object(doctor_module.tooling_module, "find_visual_cpp_executable", return_value=None):
                    with patch.object(
                        doctor_module.tooling_module.shutil,
                        "which",
                        side_effect=lambda exe: "C:\\Program Files\\dotnet\\dotnet.exe" if exe == "dotnet" else None,
                    ):
                        result = doctor_module.handle(REPO_ROOT, "windows", "doctor")

        self.assertEqual("error", result.status)
        self.assertIn("run prepare", result.text or "")
        self.assertIn("cached CMake", result.text or "")

    def test_doctor_reports_mobile_runtime_host_blockers_on_windows(self) -> None:
        doctor_module = load_module(DOCTOR_MODULE_PATH, "chaos_run_doctor_mobile_runtime_blockers")

        with patch.object(
            doctor_module.runtime_module,
            "probe_runtime",
            return_value={"isInstalled": True, "pythonPath": "artifacts/python/python.exe"},
        ):
            with patch.dict(os.environ, {"TERM": "dumb"}, clear=True):
                with patch.object(doctor_module.tooling_module, "find_cmake_executable", return_value="C:\\tools\\cmake\\bin\\cmake.exe"):
                    with patch.object(doctor_module.tooling_module, "find_visual_cpp_executable", return_value=None):
                        with patch.object(
                            doctor_module.tooling_module.shutil,
                            "which",
                            side_effect=lambda exe: "C:\\Program Files\\dotnet\\dotnet.exe" if exe == "dotnet" else None,
                        ):
                            result = doctor_module.handle(REPO_ROOT, "windows", "doctor")

        self.assertEqual("ok", result.status)
        checks = {check["name"]: check for check in result.checks}
        self.assertEqual("missing", checks["android-sdk-root"]["status"])
        self.assertEqual("missing", checks["android-ndk-root"]["status"])
        self.assertEqual("missing", checks["android-adb"]["status"])
        self.assertEqual("missing", checks["android-emulator"]["status"])
        self.assertEqual("missing", checks["ios-runtime-host"]["status"])
        self.assertIn("requires a macOS host with Xcode", checks["ios-runtime-host"]["detail"])
        self.assertIn("Mobile runtime host:", result.text or "")
        self.assertIn("Android blockers:", result.text or "")
        self.assertIn("iOS blockers:", result.text or "")

    def test_doctor_discovers_android_tooling_from_sdk_and_ndk_environment(self) -> None:
        doctor_module = load_module(DOCTOR_MODULE_PATH, "chaos_run_doctor_android_env_discovery")

        with tempfile.TemporaryDirectory() as temp_root:
            sdk_root = Path(temp_root) / "Android" / "Sdk"
            ndk_root = sdk_root / "ndk" / "26.3.11579264"
            adb_path = sdk_root / "platform-tools" / "adb.exe"
            emulator_path = sdk_root / "emulator" / "emulator.exe"

            ndk_root.mkdir(parents=True, exist_ok=False)
            adb_path.parent.mkdir(parents=True, exist_ok=False)
            emulator_path.parent.mkdir(parents=True, exist_ok=False)
            adb_path.write_text("", encoding="utf-8")
            emulator_path.write_text("", encoding="utf-8")

            with patch.object(
                doctor_module.runtime_module,
                "probe_runtime",
                return_value={"isInstalled": True, "pythonPath": "artifacts/python/python.exe"},
            ):
                with patch.dict(
                    os.environ,
                    {
                        "ANDROID_SDK_ROOT": str(sdk_root),
                        "ANDROID_NDK_ROOT": str(ndk_root),
                        "TERM": "dumb",
                    },
                    clear=True,
                ):
                    with patch.object(doctor_module.tooling_module, "find_cmake_executable", return_value="C:\\tools\\cmake\\bin\\cmake.exe"):
                        with patch.object(doctor_module.tooling_module, "find_visual_cpp_executable", return_value=None):
                            with patch.object(
                                doctor_module.tooling_module.shutil,
                                "which",
                                side_effect=lambda exe: "C:\\Program Files\\dotnet\\dotnet.exe" if exe == "dotnet" else None,
                            ):
                                result = doctor_module.handle(REPO_ROOT, "windows", "doctor")

        self.assertEqual("ok", result.status)
        checks = {check["name"]: check for check in result.checks}
        self.assertEqual("ok", checks["android-sdk-root"]["status"])
        self.assertEqual("ok", checks["android-ndk-root"]["status"])
        self.assertEqual("ok", checks["android-adb"]["status"])
        self.assertEqual("ok", checks["android-emulator"]["status"])
        self.assertIn(str(sdk_root), checks["android-sdk-root"]["detail"])
        self.assertIn(str(ndk_root), checks["android-ndk-root"]["detail"])
        self.assertIn(str(adb_path), checks["android-adb"]["detail"])
        self.assertIn(str(emulator_path), checks["android-emulator"]["detail"])


if __name__ == "__main__":
    unittest.main()
