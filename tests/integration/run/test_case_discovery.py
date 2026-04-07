from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
ADAPTERS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "adapters" / "__init__.py"
SUITE_MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "suite_manifest.py"
HELLO_WORLD_MANIFEST_PATH = REPO_ROOT / "tests" / "integration" / "run" / "fixtures" / "catalog" / "repo" / "tests" / "smoke" / "hello-world" / "suite.manifest.json"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "integration-case-discovery"


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
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_temp_root(self, prefix: str) -> Path:
        temp_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        temp_root.mkdir(parents=True, exist_ok=False)
        return temp_root

    def test_dotnet_adapter_emits_stable_synthetic_main_case(self) -> None:
        suite_manifest_module = load_module(SUITE_MANIFEST_MODULE_PATH, "chaos_run_suite_manifest_for_dotnet_cases")
        adapters_module = load_module(ADAPTERS_MODULE_PATH, "chaos_run_adapters_for_dotnet_cases")

        suite = suite_manifest_module.load_suite_manifest(HELLO_WORLD_MANIFEST_PATH)
        cases = adapters_module.create_adapter(suite).discover_cases()

        self.assertEqual(["main"], [case.case_id for case in cases])
        self.assertEqual(["main"], [case.source_id for case in cases])

    def test_case_id_collision_adds_stable_suffix(self) -> None:
        suite_manifest_module = load_module(SUITE_MANIFEST_MODULE_PATH, "chaos_run_suite_manifest_for_duplicate_cases")
        adapters_module = load_module(ADAPTERS_MODULE_PATH, "chaos_run_adapters_for_duplicate_cases")

        temp_root = self._make_temp_root("duplicate-cases")
        try:
            manifest_path = temp_root / "tests" / "integration" / "duplicate-cases" / "suite.manifest.json"
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
        finally:
            shutil.rmtree(temp_root, ignore_errors=True)

        self.assertEqual(["duplicate", "duplicate-34a7"], [case.case_id for case in cases])


if __name__ == "__main__":
    unittest.main()
