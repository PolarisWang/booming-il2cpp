from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
TEST_COMMAND_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "test.py"


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


class PythonUnittestCommandTests(unittest.TestCase):
    def test_python_unittest_kind_runs_module_with_current_python(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_python_unittest")
        completed = SimpleNamespace(returncode=0, stdout="ok\n", stderr="")

        with patch.object(test_module, "run_process", return_value=completed) as run_mock:
            result = test_module.handle(
                {
                    "id": "test-contract-managed-closure-bundle",
                    "handler": "test.dispatch",
                    "kind": "python-unittest",
                    "target": "managed-closure-bundle",
                    "test_module": "tests.unit.run.test_managed_closure_contract_bundle",
                    "artifacts": ["tests/contracts/analysis/v0/samples"],
                },
                REPO_ROOT,
                "windows",
                "test contract managed-closure-bundle",
            )

        self.assertEqual("ok", result.status)
        run_mock.assert_called_once_with(
            [sys.executable, "-m", "unittest", "tests.unit.run.test_managed_closure_contract_bundle"],
            cwd=REPO_ROOT,
        )
        self.assertEqual(
            [str((REPO_ROOT / "tests" / "contracts" / "analysis" / "v0" / "samples").resolve())],
            result.payload["artifacts"],
        )


if __name__ == "__main__":
    unittest.main()
