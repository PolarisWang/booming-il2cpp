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
    def test_native_contract_build_resets_binary_dir_before_configure(self) -> None:
        build_module = load_module(BUILD_MODULE_PATH, "chaos_run_build")
        command = {
            "kind": "native-contract",
            "target": "abi",
            "source_dir": "tests/contracts/native/abi",
            "binary_dir": "artifacts/run/native-contract-abi",
        }
        completed = subprocess.CompletedProcess(["cmake"], 0, "", "")

        with patch.object(build_module.tooling_module, "cmake_environment", return_value=("cmake", {})):
            with patch.object(build_module, "run_process", return_value=completed):
                with patch.object(build_module, "_reset_binary_dir") as reset_binary_dir:
                    result = build_module.handle(command, REPO_ROOT, "windows", "build native-contract abi")

        self.assertEqual("ok", result.status)
        reset_binary_dir.assert_called_once_with(REPO_ROOT / "artifacts" / "run" / "native-contract-abi")
        self.assertEqual("Run completed: build native-contract abi\n", result.text)
        self.assertIn("consoleText", result.payload)


if __name__ == "__main__":
    unittest.main()
