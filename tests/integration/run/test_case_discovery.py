from __future__ import annotations

import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
ADAPTERS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "adapters" / "__init__.py"
SUITE_MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "suite_manifest.py"
HELLO_WORLD_MANIFEST_PATH = REPO_ROOT / "tests" / "smoke" / "hello-world" / "suite.manifest.json"


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


class CaseDiscoveryTests(unittest.TestCase):
    def test_dotnet_adapter_emits_stable_synthetic_main_case(self) -> None:
        suite_manifest_module = load_module(SUITE_MANIFEST_MODULE_PATH, "booming_run_suite_manifest_for_dotnet_cases")
        adapters_module = load_module(ADAPTERS_MODULE_PATH, "booming_run_adapters_for_dotnet_cases")

        suite = suite_manifest_module.load_suite_manifest(HELLO_WORLD_MANIFEST_PATH)
        cases = adapters_module.create_adapter(suite).discover_cases()

        self.assertEqual(["main"], [case.case_id for case in cases])
        self.assertEqual(["main"], [case.source_id for case in cases])

    def test_case_id_collision_adds_stable_suffix(self) -> None:
        suite_manifest_module = load_module(SUITE_MANIFEST_MODULE_PATH, "booming_run_suite_manifest_for_duplicate_cases")
        adapters_module = load_module(ADAPTERS_MODULE_PATH, "booming_run_adapters_for_duplicate_cases")

        with tempfile.TemporaryDirectory() as temp_dir:
            manifest_path = Path(temp_dir) / "tests" / "integration" / "duplicate-cases" / "suite.manifest.json"
            manifest_path.parent.mkdir(parents=True, exist_ok=True)
            manifest_path.write_text(
                json.dumps(
                    {
                        "displayName": "Duplicate Cases",
                        "runnerType": "shell-script",
                        "supportedStages": ["run", "all"],
                        "runnerConfig": {
                            "caseSources": [
                                {"sourceId": "dup-a", "sourceName": "Duplicate"},
                                {"sourceId": "dup-b", "sourceName": "Duplicate"},
                            ]
                        },
                    }
                ),
                encoding="utf-8",
            )

            suite = suite_manifest_module.load_suite_manifest(manifest_path)
            cases = adapters_module.create_adapter(suite).discover_cases()

        self.assertEqual(["duplicate", "duplicate-34a7"], [case.case_id for case in cases])


if __name__ == "__main__":
    unittest.main()
