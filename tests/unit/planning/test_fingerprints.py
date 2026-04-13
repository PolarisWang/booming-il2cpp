from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
FINGERPRINTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "fingerprints.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "fingerprints"


def load_fingerprints_module():
    if not FINGERPRINTS_MODULE_PATH.is_file():
        raise FileNotFoundError(f"fingerprints module missing: {FINGERPRINTS_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("chaos_run_fingerprints", FINGERPRINTS_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load fingerprints module: {FINGERPRINTS_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class FingerprintsTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def test_fingerprint_build_cache_uses_controlled_layout_and_current_pointer(self) -> None:
        fingerprints_module = load_fingerprints_module()
        suite_id = "fixture-suite"
        source_root = "subjects/FixtureSuite/source"

        repo_root = TEST_TMP_ROOT / f"repo-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        try:
            result = fingerprints_module.materialize_fingerprint(
                repo_root=repo_root,
                host_platform="macos",
                family="smoke",
                suite=suite_id,
                fingerprint_inputs={"sourceRoots": [source_root], "toolchainFiles": ["global.json"]},
            )

            self.assertIn(f"/artifacts/tests/build/macos/smoke/{suite_id}/fingerprints/", result["fingerprintDir"])
            current_payload = json.loads((repo_root / result["currentPath"]).read_text(encoding="utf-8"))
            self.assertEqual(result["fingerprint"], current_payload["fingerprint"])
            self.assertEqual(result["fingerprintDir"], current_payload["fingerprintDir"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
