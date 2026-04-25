from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SUITE_MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "suite_manifest.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "suite-manifest"


def load_suite_manifest_module():
    if not SUITE_MANIFEST_MODULE_PATH.is_file():
        raise FileNotFoundError(f"suite manifest module missing: {SUITE_MANIFEST_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("chaos_run_suite_manifest", SUITE_MANIFEST_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load suite manifest module: {SUITE_MANIFEST_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def write_suite_manifest(root: Path, *, family: str, suite_id: str, display_name: str) -> Path:
    manifest_path = root / "tests" / family / suite_id / "suite.manifest.json"
    manifest_path.parent.mkdir(parents=True, exist_ok=True)
    manifest_path.write_text(
        json.dumps(
            {
                "displayName": display_name,
                "runnerType": "dotnet-app",
                "supportedStages": ["build", "run", "all"],
                "runnerConfig": {
                    "projectPath": f"verification/catalog/owners/{display_name}/support/host/{display_name}.csproj"
                },
                "supportedHosts": ["windows", "macos"],
            }
        ),
        encoding="utf-8",
    )
    return manifest_path


class SuiteManifestTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def test_load_suite_manifest_reads_runner_metadata_from_path(self) -> None:
        suite_manifest_module = load_suite_manifest_module()
        family = "smoke"
        suite_id = "fixture-suite"
        display_name = "FixtureSuite"

        repo_root = TEST_TMP_ROOT / f"repo-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            manifest_path = write_suite_manifest(
                repo_root,
                family=family,
                suite_id=suite_id,
                display_name=display_name,
            )
            manifest = suite_manifest_module.load_suite_manifest(manifest_path)

            self.assertEqual(family, manifest.family)
            self.assertEqual(suite_id, manifest.suite_id)
            self.assertEqual(display_name, manifest.display_name)
            self.assertEqual("dotnet-app", manifest.runner_type)
            self.assertEqual(["build", "run", "all"], manifest.supported_stages)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_load_suite_manifest_rejects_missing_runner_type(self) -> None:
        suite_manifest_module = load_suite_manifest_module()

        repo_root = TEST_TMP_ROOT / f"repo-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            manifest_path = repo_root / "tests" / "contract" / "bad-manifest" / "suite.manifest.json"
            manifest_path.parent.mkdir(parents=True, exist_ok=True)
            manifest_path.write_text(
                json.dumps(
                    {
                        "displayName": "Bad Manifest",
                        "supportedStages": ["run"],
                        "runnerConfig": {},
                    }
                ),
                encoding="utf-8",
            )

            with self.assertRaisesRegex(ValueError, "runnerType"):
                suite_manifest_module.load_suite_manifest(manifest_path)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
