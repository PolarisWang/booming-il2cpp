from __future__ import annotations

import importlib.util
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
TOOLING_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "core" / "tooling.py"


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


class AndroidBootstrapTests(unittest.TestCase):
    def test_download_file_uses_user_agent_header(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_android_download_headers")
        captured_headers: dict[str, str | None] = {}

        class FakeResponse:
            def __init__(self) -> None:
                self._served = False

            def __enter__(self):
                return self

            def __exit__(self, exc_type, exc, tb) -> None:
                return None

            def read(self, size: int = -1) -> bytes:
                del size
                if self._served:
                    return b""
                self._served = True
                return b"payload"

        def fake_urlopen(request):
            captured_headers["User-Agent"] = request.headers.get("User-agent") or request.headers.get("User-Agent")
            return FakeResponse()

        with tempfile.TemporaryDirectory() as temp_root:
            destination = Path(temp_root) / "download.bin"
            with patch.object(tooling_module.urllib.request, "urlopen", side_effect=fake_urlopen):
                tooling_module._download_file("https://example.com/tool.zip", destination)

            self.assertEqual("payload", destination.read_text(encoding="utf-8"))
            self.assertTrue(captured_headers["User-Agent"])

    def test_android_environment_overrides_preserve_system_path(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_android_env_path_merge")

        with tempfile.TemporaryDirectory() as temp_root:
            repo_root = Path(temp_root)
            java_bin = repo_root / "artifacts" / "toolchains" / "android" / "jdk" / "bin"
            sdkmanager_dir = repo_root / "artifacts" / "toolchains" / "android" / "sdk" / "cmdline-tools" / "latest" / "bin"
            adb_dir = repo_root / "artifacts" / "toolchains" / "android" / "sdk" / "platform-tools"
            emulator_dir = repo_root / "artifacts" / "toolchains" / "android" / "sdk" / "emulator"
            ndk_dir = repo_root / "artifacts" / "toolchains" / "android" / "sdk" / "ndk" / tooling_module.ANDROID_NDK_VERSION

            java_bin.mkdir(parents=True, exist_ok=True)
            sdkmanager_dir.mkdir(parents=True, exist_ok=True)
            adb_dir.mkdir(parents=True, exist_ok=True)
            emulator_dir.mkdir(parents=True, exist_ok=True)
            ndk_dir.mkdir(parents=True, exist_ok=True)

            (java_bin / "java.exe").write_text("", encoding="utf-8")
            (sdkmanager_dir / "sdkmanager.bat").write_text("", encoding="utf-8")
            (adb_dir / "adb.exe").write_text("", encoding="utf-8")
            (emulator_dir / "emulator.exe").write_text("", encoding="utf-8")

            with patch.dict(tooling_module.os.environ, {"PATH": r"C:\Windows\System32"}, clear=False):
                env = tooling_module.android_environment_overrides(repo_root)

        self.assertIn(r"C:\Windows\System32", env["PATH"])

    def test_windows_session_can_bootstrap_android_host_tooling_into_repo_cache(self) -> None:
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_run_tooling_android_host_bootstrap")
        repo_root = REPO_ROOT / "artifacts" / ".tmp-tests" / "android-host-bootstrap"
        if repo_root.exists():
            tooling_module.shutil.rmtree(repo_root, ignore_errors=True)
        repo_root.mkdir(parents=True, exist_ok=True)

        downloads: list[str] = []
        sdkmanager_commands: list[list[str]] = []

        def fake_download(url: str, destination: Path) -> None:
            downloads.append(url)
            destination.parent.mkdir(parents=True, exist_ok=True)
            destination.write_text("archive", encoding="utf-8")

        def fake_extract_zip(archive_path: Path, destination: Path) -> None:
            destination.mkdir(parents=True, exist_ok=True)
            archive_name = archive_path.name
            if "OpenJDK" in archive_name or "jdk" in archive_name.lower():
                java_home = destination / "jdk-17.0.0"
                (java_home / "bin").mkdir(parents=True, exist_ok=True)
                (java_home / "bin" / "java.exe").write_text("", encoding="utf-8")
                return

            cmdline_root = destination / "cmdline-tools"
            (cmdline_root / "bin").mkdir(parents=True, exist_ok=True)
            (cmdline_root / "lib").mkdir(parents=True, exist_ok=True)
            (cmdline_root / "bin" / "sdkmanager.bat").write_text("", encoding="utf-8")

        def fake_run_android_tool(
            arguments: list[str],
            *,
            env: dict[str, str] | None = None,
            input_text: str | None = None,
            cwd: Path | None = None,
        ) -> subprocess.CompletedProcess[str]:
            del env, input_text, cwd
            sdkmanager_commands.append(arguments)

            sdk_root_arg = next(argument for argument in arguments if argument.startswith("--sdk_root="))
            sdk_root = Path(sdk_root_arg.split("=", 1)[1])
            (sdk_root / "platform-tools").mkdir(parents=True, exist_ok=True)
            (sdk_root / "platform-tools" / "adb.exe").write_text("", encoding="utf-8")
            (sdk_root / "emulator").mkdir(parents=True, exist_ok=True)
            (sdk_root / "emulator" / "emulator.exe").write_text("", encoding="utf-8")
            (sdk_root / "ndk" / tooling_module.ANDROID_NDK_VERSION).mkdir(parents=True, exist_ok=True)
            return subprocess.CompletedProcess(arguments, 0, "android sdkmanager ok", "")

        outcome = tooling_module.ensure_android_host_tooling_available(
            "prepare android-host",
            "windows",
            repo_root,
            download_file=fake_download,
            extract_zip=fake_extract_zip,
            run_android_tool=fake_run_android_tool,
        )

        self.assertTrue(outcome.ready)
        self.assertIn("android sdkmanager ok", outcome.output)
        self.assertEqual(2, len(downloads))
        self.assertTrue(any("commandlinetools-win" in url for url in downloads))
        self.assertTrue(any("adoptium" in url.lower() or "openjdk" in url.lower() for url in downloads))
        self.assertEqual(2, len(sdkmanager_commands))

        env_overrides = tooling_module.android_environment_overrides(repo_root)
        self.assertEqual(str(repo_root / "artifacts" / "toolchains" / "android" / "sdk"), env_overrides["ANDROID_SDK_ROOT"])
        self.assertEqual(
            str(repo_root / "artifacts" / "toolchains" / "android" / "sdk" / "ndk" / tooling_module.ANDROID_NDK_VERSION),
            env_overrides["ANDROID_NDK_ROOT"],
        )
        self.assertIn("java.exe", env_overrides["PATH"])
