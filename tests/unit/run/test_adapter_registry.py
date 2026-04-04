from __future__ import annotations

import importlib.util
import sys
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


class AdapterRegistryTests(unittest.TestCase):
    def test_create_adapter_returns_dotnet_app_adapter_for_smoke_manifest(self) -> None:
        suite_manifest_module = load_module(SUITE_MANIFEST_MODULE_PATH, "booming_run_suite_manifest_for_adapter_registry")
        adapters_module = load_module(ADAPTERS_MODULE_PATH, "booming_run_adapters_registry")

        suite = suite_manifest_module.load_suite_manifest(HELLO_WORLD_MANIFEST_PATH)
        adapter = adapters_module.create_adapter(suite)

        self.assertEqual("dotnet-app", adapter.runner_type)
        self.assertEqual("DotnetAppAdapter", adapter.__class__.__name__)

    def test_supported_runner_types_match_design_contract(self) -> None:
        adapters_module = load_module(ADAPTERS_MODULE_PATH, "booming_run_adapters_registry_contract")

        self.assertEqual(
            ["dotnet-app", "native-cmake", "python-pytest", "shell-script", "workflow"],
            sorted(adapters_module.supported_runner_types()),
        )


if __name__ == "__main__":
    unittest.main()
