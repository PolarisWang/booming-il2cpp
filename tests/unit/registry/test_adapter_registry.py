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
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "adapter-registry"


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


def write_suite_manifest(root: Path, *, family: str, suite_id: str, display_name: str, runner_type: str) -> Path:
    manifest_path = root / "tests" / family / suite_id / "suite.manifest.json"
    manifest_path.parent.mkdir(parents=True, exist_ok=True)
    manifest_path.write_text(
        json.dumps(
            {
                "displayName": display_name,
                "runnerType": runner_type,
                "supportedStages": ["build", "run", "all"],
                "runnerConfig": {
                    "projectPath": f"verification/catalog/owners/{display_name}/support/host/{display_name}.csproj"
                },
            }
        ),
        encoding="utf-8",
    )
    return manifest_path


class AdapterRegistryTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def test_create_adapter_returns_dotnet_app_adapter_for_smoke_manifest(self) -> None:
        suite_manifest_module = load_module(SUITE_MANIFEST_MODULE_PATH, "chaos_run_suite_manifest_for_adapter_registry")
        adapters_module = load_module(ADAPTERS_MODULE_PATH, "chaos_run_adapters_registry")

        repo_root = TEST_TMP_ROOT / f"repo-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            manifest_path = write_suite_manifest(
                repo_root,
                family="smoke",
                suite_id="fixture-suite",
                display_name="FixtureSuite",
                runner_type="dotnet-app",
            )
            suite = suite_manifest_module.load_suite_manifest(manifest_path)
            adapter = adapters_module.create_adapter(suite)

            self.assertEqual("dotnet-app", adapter.runner_type)
            self.assertEqual("DotnetAppAdapter", adapter.__class__.__name__)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_supported_runner_types_match_design_contract(self) -> None:
        adapters_module = load_module(ADAPTERS_MODULE_PATH, "chaos_run_adapters_registry_contract")

        self.assertEqual(
            ["dotnet-app", "native-cmake", "python-pytest", "shell-script", "workflow"],
            sorted(adapters_module.supported_runner_types()),
        )


if __name__ == "__main__":
    unittest.main()
