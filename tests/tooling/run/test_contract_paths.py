from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
CONTRACTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "contracts.py"


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


class ContractPathsTests(unittest.TestCase):
    def test_analysis_contract_paths_use_canonical_contract_roots(self) -> None:
        contracts_module = load_module(CONTRACTS_MODULE_PATH, "chaos_run_contract_paths_analysis")

        paths = contracts_module.analysis_contract_json_paths(REPO_ROOT)

        self.assertIn(
            REPO_ROOT / "contracts" / "artifacts" / "v0" / "schemas" / "aot-manifest.schema.json",
            paths,
        )
        self.assertIn(
            REPO_ROOT / "tests" / "contracts" / "analysis" / "v0" / "samples" / "aot-manifest.min.json",
            paths,
        )
        self.assertIn(
            REPO_ROOT / "tests" / "contracts" / "analysis" / "v0" / "snapshots" / "aot-manifest.snapshot.json", paths)
        self.assertNotIn(REPO_ROOT / "analysis" / "contracts" / "schemas" / "aot-manifest.schema.json", paths)
        self.assertNotIn(REPO_ROOT / "analysis" / "contracts" / "examples" / "aot-manifest.min.json", paths)
        self.assertNotIn(
            REPO_ROOT / "contracts" / "artifacts" / "v0" / "samples" / "aot-manifest.min.json",
            paths,
        )
        self.assertNotIn(REPO_ROOT / "tests" / "contract" / "schema" / "aot-manifest.snapshot.json", paths)

    def test_trace_contract_paths_use_canonical_tests_contracts_trace(self) -> None:
        contracts_module = load_module(CONTRACTS_MODULE_PATH, "chaos_run_contract_paths_trace")

        paths = contracts_module.trace_contract_json_paths(REPO_ROOT)

        self.assertIn(
            REPO_ROOT / "tests" / "contracts" / "trace" / "schema" / "warmup-trace.schema.json",
            paths,
        )
        self.assertNotIn(
            REPO_ROOT / "tests" / "contract" / "trace" / "schema" / "warmup-trace.schema.json",
            paths,
        )


if __name__ == "__main__":
    unittest.main()
