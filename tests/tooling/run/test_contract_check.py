from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path
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


class ContractCheckTests(unittest.TestCase):
    def test_analysis_contract_check_runs_parse_and_validation(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_contract_analysis")

        with patch.object(
            test_module.contracts_module,
            "analysis_contract_json_paths",
            return_value=[REPO_ROOT / "contracts" / "artifacts" / "v0" / "schemas" / "aot-manifest.schema.json"],
        ) as paths_mock:
            with patch.object(test_module.contracts_module, "assert_json_files_parse") as parse_mock:
                with patch.object(test_module.contracts_module, "validate_analysis_contracts") as validate_mock:
                    result = test_module.handle(
                        {
                            "id": "test-contract-analysis-schema",
                            "handler": "test.dispatch",
                            "kind": "contract-check",
                            "target": "analysis-schema",
                            "contract_check": "analysis-schema",
                        },
                        REPO_ROOT,
                        "macos",
                        "test contract analysis-schema",
                    )

        self.assertEqual("ok", result.status)
        paths_mock.assert_called_once_with(REPO_ROOT)
        parse_mock.assert_called_once()
        validate_mock.assert_called_once_with(REPO_ROOT)

    def test_trace_schema_contract_check_surfaces_precise_failure(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_run_test_command_contract_trace_failure")

        with patch.object(test_module.contracts_module, "trace_contract_json_paths", return_value=[]):
            with patch.object(test_module.contracts_module, "assert_json_files_parse"):
                with patch.object(
                    test_module.contracts_module,
                    "validate_trace_schema_contracts",
                    side_effect=RuntimeError("schema required property missing at windows-warmup-trace.snapshot.json: 'phase'"),
                ):
                    result = test_module.handle(
                        {
                            "id": "test-contract-trace-schema",
                            "handler": "test.dispatch",
                            "kind": "contract-check",
                            "target": "trace-schema",
                            "contract_check": "trace-schema",
                        },
                        REPO_ROOT,
                        "windows",
                        "test contract trace-schema",
                    )

        self.assertEqual("error", result.status)
        self.assertIn("windows-warmup-trace.snapshot.json", result.text)
        self.assertIn("schema required property missing", result.text)


if __name__ == "__main__":
    unittest.main()
