from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SELECTORS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "selectors.py"


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


class SelectorTests(unittest.TestCase):
    def test_engineering_selector_normalizes_subject_and_kind(self) -> None:
        selectors_module = load_module(SELECTORS_MODULE_PATH, "chaos_selector_engineering")

        normalized = selectors_module.normalize_selector_options(
            "engineering-workload",
            {"subject": "SolutionCorePack", "kind": "codegen"},
        )

        self.assertEqual("engineering-workload/SolutionCorePack/codegen", normalized["id"])
        self.assertEqual("SolutionCorePack", normalized["subject"])
        self.assertEqual("codegen", normalized["kind"])

    def test_declared_selector_parses_stable_id(self) -> None:
        selectors_module = load_module(SELECTORS_MODULE_PATH, "chaos_selector_declared")
        declared_id = "declared-unit-test/SolutionCorePack::MainlineFeaturePack::MainlineFeaturePack.ArrayOpsProofEntry::Run()"

        normalized = selectors_module.normalize_selector_options(
            "declared-unit-test",
            {"id": declared_id},
        )

        self.assertEqual(declared_id, normalized["id"])
        self.assertEqual(
            "SolutionCorePack::MainlineFeaturePack::MainlineFeaturePack.ArrayOpsProofEntry::Run()",
            normalized["stable_id"],
        )


if __name__ == "__main__":
    unittest.main()
