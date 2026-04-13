from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
REGISTRY_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "registry.py"
PUBLIC_SPECS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "public_specs.py"


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


class SolutionCorePackRegistryTests(unittest.TestCase):
    def test_registry_scan_surfaces_solution_core_pack_subject(self) -> None:
        registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_run_registry_solution_core_pack")
        specs_module = load_module(PUBLIC_SPECS_MODULE_PATH, "chaos_run_public_specs_solution_core_pack")

        index = registry_module.scan_registry(
            REPO_ROOT,
            host_platform="windows",
            public_suite_specs=specs_module.PUBLIC_TEST_SPECS,
        )

        subject_item = next(item for item in index.flat_items if item["id"] == "subject/SolutionCorePack")
        self.assertEqual("subject", subject_item["type"])
        self.assertEqual("canonical", subject_item["category"])
        self.assertEqual("correctness.dev", subject_item["defaultGoalId"])
        self.assertEqual("windows-native-check", subject_item["defaultMatrixId"])
        self.assertEqual("dotnet-solution", subject_item["sourceModel"])


if __name__ == "__main__":
    unittest.main()
