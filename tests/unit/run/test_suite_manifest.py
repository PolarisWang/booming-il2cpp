from __future__ import annotations

import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SUITE_MANIFEST_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "suite_manifest.py"
HELLO_WORLD_MANIFEST_PATH = REPO_ROOT / "tests" / "smoke" / "hello-world" / "suite.manifest.json"


def load_suite_manifest_module():
    if not SUITE_MANIFEST_MODULE_PATH.is_file():
        raise FileNotFoundError(f"suite manifest module missing: {SUITE_MANIFEST_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("booming_run_suite_manifest", SUITE_MANIFEST_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load suite manifest module: {SUITE_MANIFEST_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class SuiteManifestTests(unittest.TestCase):
    def test_load_suite_manifest_reads_runner_metadata_from_path(self) -> None:
        suite_manifest_module = load_suite_manifest_module()

        manifest = suite_manifest_module.load_suite_manifest(HELLO_WORLD_MANIFEST_PATH)

        self.assertEqual("smoke", manifest.family)
        self.assertEqual("hello-world", manifest.suite_id)
        self.assertEqual("HelloWorld", manifest.display_name)
        self.assertEqual("dotnet-app", manifest.runner_type)
        self.assertEqual(["build", "run", "all"], manifest.supported_stages)

    def test_load_suite_manifest_rejects_missing_runner_type(self) -> None:
        suite_manifest_module = load_suite_manifest_module()

        with tempfile.TemporaryDirectory() as temp_dir:
            manifest_path = Path(temp_dir) / "tests" / "contract" / "bad-manifest" / "suite.manifest.json"
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


if __name__ == "__main__":
    unittest.main()
