from __future__ import annotations

import importlib.util
import subprocess
import sys
import unittest
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
COMMON_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "common.py"


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


class RunProcessTests(unittest.TestCase):
    def test_run_process_captures_binary_output_for_custom_decoding(self) -> None:
        common_module = load_module(COMMON_MODULE_PATH, "chaos_run_common")

        with patch.object(
            common_module.subprocess,
            "run",
            return_value=subprocess.CompletedProcess(["dotnet", "--info"], 0, b"", b""),
        ) as subprocess_run:
            common_module.run_process(["dotnet", "--info"], cwd=REPO_ROOT)

        kwargs = subprocess_run.call_args.kwargs
        self.assertFalse(kwargs["text"])

    def test_run_process_prefers_utf8_when_output_is_utf8_encoded(self) -> None:
        common_module = load_module(COMMON_MODULE_PATH, "chaos_run_common_utf8")
        payload = "适用于 .NET Framework MSBuild".encode("utf-8")

        with patch.object(common_module.locale, "getpreferredencoding", return_value="gbk"):
            with patch.object(
                common_module.subprocess,
                "run",
                return_value=subprocess.CompletedProcess(["msbuild"], 0, payload, b""),
            ):
                completed = common_module.run_process(["msbuild"], cwd=REPO_ROOT)

        self.assertIn("适用于 .NET Framework MSBuild", completed.stdout)

    def test_run_process_merges_windows_environment_without_duplicate_path_keys(self) -> None:
        common_module = load_module(COMMON_MODULE_PATH, "chaos_run_common_windows_env")

        with patch.object(common_module.os, "name", "nt"):
            with patch.dict(common_module.os.environ, {"Path": "C:\\Windows\\System32"}, clear=True):
                with patch.object(
                    common_module.subprocess,
                    "run",
                    return_value=subprocess.CompletedProcess(["powershell"], 0, b"", b""),
                ) as subprocess_run:
                    common_module.run_process(["powershell"], cwd=REPO_ROOT, env={"PATH": "E:\\tools\\cmake"})

        passed_env = subprocess_run.call_args.kwargs["env"]
        self.assertEqual("E:\\tools\\cmake", passed_env["PATH"])
        self.assertNotIn("Path", passed_env)


if __name__ == "__main__":
    unittest.main()
