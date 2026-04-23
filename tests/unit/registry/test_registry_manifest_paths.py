from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
REGISTRY_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "registry.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "registry-manifest-paths"


def load_registry_module():
    if not REGISTRY_MODULE_PATH.is_file():
        raise FileNotFoundError(f"registry module missing: {REGISTRY_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("chaos_run_registry_manifest_paths", REGISTRY_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load registry module: {REGISTRY_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def write_json(path: Path, payload: dict[str, object]) -> Path:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload), encoding="utf-8")
    return path


class RegistryManifestPathTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self, prefix: str) -> Path:
        repo_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def test_module_manifest_accepts_canonical_fixture_root(self) -> None:
        registry_module = load_registry_module()
        repo_root = self._make_repo_root("module-canonical")
        try:
            manifest_path = write_json(
                repo_root / "tests" / "fixtures" / "registry" / "modules" / "analysis" / "basic" / "verification.manifest.json",
                {
                    "displayName": "Analysis Basic",
                    "moduleIds": ["analysis"],
                    "supportedHosts": ["windows"],
                    "members": [{"type": "suite", "id": "contract/analysis-schema"}],
                },
            )

            item = registry_module._load_module_manifest(manifest_path)

            self.assertEqual("module/analysis/basic", item["id"])
            self.assertEqual(str(manifest_path), item["manifestPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_system_manifest_accepts_canonical_fixture_root(self) -> None:
        registry_module = load_registry_module()
        repo_root = self._make_repo_root("system-canonical")
        try:
            manifest_path = write_json(
                repo_root / "tests" / "fixtures" / "registry" / "systems" / "runtime-baseline-macos" / "scenario.manifest.json",
                {
                    "displayName": "Runtime Baseline macOS",
                    "scenarioId": "runtime-baseline-macos",
                    "supportedHosts": ["macos"],
                    "members": [{"type": "module", "id": "module/analysis/basic"}],
                },
            )

            item = registry_module._load_system_manifest(manifest_path)

            self.assertEqual("system/runtime-baseline-macos", item["id"])
            self.assertEqual(str(manifest_path), item["manifestPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_pipeline_manifest_loader_is_removed_after_verification_v1_cutover(self) -> None:
        registry_module = load_registry_module()
        self.assertFalse(hasattr(registry_module, "_load_pipeline_manifest"))

    def test_legacy_registry_paths_are_rejected_after_phase8_cutover(self) -> None:
        registry_module = load_registry_module()
        repo_root = self._make_repo_root("legacy-compatible")
        try:
            module_manifest_path = write_json(
                repo_root / "tests" / "registry" / "modules" / "analysis" / "basic" / "verification.manifest.json",
                {
                    "displayName": "Analysis Basic",
                    "moduleIds": ["analysis"],
                    "supportedHosts": ["windows"],
                    "members": [{"type": "suite", "id": "contract/analysis-schema"}],
                },
            )
            system_manifest_path = write_json(
                repo_root / "tests" / "registry" / "system" / "runtime-baseline-macos" / "scenario.manifest.json",
                {
                    "displayName": "Runtime Baseline macOS",
                    "scenarioId": "runtime-baseline-macos",
                    "supportedHosts": ["macos"],
                    "members": [{"type": "module", "id": "module/analysis/basic"}],
                },
            )
            with self.assertRaises(ValueError):
                registry_module._load_module_manifest(module_manifest_path)

            with self.assertRaises(ValueError):
                registry_module._load_system_manifest(system_manifest_path)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
